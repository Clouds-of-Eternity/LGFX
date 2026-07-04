#include "ShaderCompiler.hpp"
#include "Json.hpp"
#include "DenseSet.hpp"
#include "BinaryIO.hpp"
#include "Scope.hpp"
#include "limits.h"
#include "Path.hpp"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

slang::IGlobalSession *globalSession;
bool AssetcShaderCompilerInitialize()
{
    auto createSessionResult = slang::createGlobalSession(&globalSession);
    if (createSessionResult != SLANG_OK)
    {
        globalSession = NULL;
        return false;
    }
    return true;
}
void AssetcShaderCompilerUnload()
{
    if (globalSession != NULL)
    {
        //globalSession->Release();
        slang::shutdown();
    }
}

enum ShaderFunctionResourceType
{
    ShaderFunctionResourceType_Uniform,
    ShaderFunctionResourceType_Sampler,
    ShaderFunctionResourceType_Texture,
    ShaderFunctionResourceType_StructuredBuffer,
    ShaderFunctionResourceType_InputAttachment,
    ShaderFunctionResourceType_StorageTexture,
    ShaderFunctionResourceType_Unknown = 0xFFFFFFFF
};
enum ShaderFunctionStage
{
    ShaderFunctionStage_Vertex,
    ShaderFunctionStage_Fragment,
    ShaderFunctionStage_Compute
};

usize ShaderCompiler_ExtractSpirvFromSFNFilePath(const char *name, text outputPathNoFileExtension)
{
    FILE *fs = fopen(name, "rb");
    if (fs == NULL)
    {
        return 1;   
    }
    usize errorCode = ShaderCompiler_ExtractSpirvFromSFN(GetFileDataStream(fs), outputPathNoFileExtension);
    fclose(fs);
    return errorCode;
}
usize ShaderCompiler_ExtractSpirvFromSFN(IDataStream input, text outputPathNoFileExtension)
{
    const u32 fileVersion = input.Read<u32>();
    if (fileVersion == 1)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        Scope(ArenaAllocator, arena);

        const u32 shaderType = input.Read<u32>();
        const u32 maxSets = input.Read<u32>();
        for (u32 j = 0; j < maxSets; j++)
        {
            const u32 paramCount = input.Read<u32>();

            for (u32 i = 0; i < paramCount; i++)
            {
                input.PassString(); //name
                input.Read<u32>(); //binding index
                
                input.Read<u32>(); //array length

                ShaderFunctionResourceType resourceType = (ShaderFunctionResourceType)input.Read<u32>();
                if (resourceType == ShaderFunctionResourceType_Uniform)
                {
                    input.Read<u32>(); //size
                }
                else if (resourceType == ShaderFunctionResourceType_Sampler)
                {
                }
                else if (resourceType == ShaderFunctionResourceType_Texture)
                {
                }
                else if (resourceType == ShaderFunctionResourceType_StorageTexture)
                {
                }
                else if (resourceType == ShaderFunctionResourceType_StructuredBuffer)
                {
                }
                else if (resourceType == ShaderFunctionResourceType_InputAttachment)
                {
                }
                else
                {
                    return 1;
                }
            }
        }
        
        usize result = 0;
        if (shaderType == 0)
        {
            //vertex-fragment
            ShaderFunctionStage stage1 = (ShaderFunctionStage)input.Read<u32>();
            if (stage1 != ShaderFunctionStage_Vertex)
            {
                return 1;
            }
            usize lenBytes = input.Read<u32>();
            usize module1DataLength = lenBytes / 4;
            u32 *module1Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
            input.ReadByteArray((u8*)module1Data, lenBytes);
            
            ShaderFunctionStage stage2 = (ShaderFunctionStage)input.Read<u32>();
            if (stage2 != ShaderFunctionStage_Fragment)
            {
                return 1;
            }
            lenBytes = input.Read<u32>();
            usize module2DataLength = lenBytes / 4;
            u32 *module2Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
            input.ReadByteArray((u8*)module2Data, lenBytes);

            string outputPath1 = string::Format(arena.AsAllocator(), "%s%s", outputPathNoFileExtension, ".vert");
            string outputPath2 = string::Format(arena.AsAllocator(), "%s%s", outputPathNoFileExtension, ".frag");

            FILE *fs = fopen(outputPath1.buffer, "wb");
            if (fs != NULL)
            {
                fwrite(module1Data, 4, module1DataLength, fs);
                fclose(fs);
            }
            else result = 1;
            fs = fopen(outputPath2.buffer, "wb");
            if (fs != NULL)
            {
                fwrite(module2Data, 4, module2DataLength, fs);
                fclose(fs);
            }
            else result = 1;
        }
        else if (shaderType == 1)
        {
            //compute
            ShaderFunctionStage stage1 = (ShaderFunctionStage)input.Read<u32>();
            if (stage1 != ShaderFunctionStage_Compute)
            {
                return 1;
            }
            usize lenBytes = input.Read<u32>();

            usize module1DataLength = lenBytes / 4;
            u32 *module1Data = (u32 *)arena.AsAllocator().Allocate(lenBytes);
            input.ReadByteArray((u8*)module1Data, lenBytes);

            string outputPath1 = string::Format(arena.AsAllocator(), "%s%s", outputPathNoFileExtension, ".vert");
            FILE *fs = fopen(outputPath1.buffer, "wb");
            if (fs != NULL)
            {
                fwrite(module1Data, 4, module1DataLength, fs);
                fclose(fs);
            }
            else result = 1;
        }
        return result;
    }
    else
    {
        return 1;
    }
}

ShaderCompiler *ShaderCompiler_Create(const ShaderCompilerCreateInfo *createInfo)
{
    assert(globalSession != NULL);
    ShaderCompiler *result = (ShaderCompiler *)DEFAULT_ALLOC(sizeof(ShaderCompiler));
    *result = ShaderCompiler(GetCAllocator(), createInfo->numDirectories, createInfo->objectDirectories != NULL);

    for (u32 i = 0; i < createInfo->numDirectories; i++)
    {
        result->sourceDirectories[i] = string(result->allocator, createInfo->sourceDirectories[i]);
        if (createInfo->outputDirectories != NULL)
        {
            result->outputDirectories[i] = string(result->allocator, createInfo->outputDirectories[i]);
        }
        else
        {
            result->outputDirectories[i] = result->sourceDirectories[i].Clone(result->allocator);
        }

        if (createInfo->objectDirectories != NULL)
        {
            result->objectDirectories[i] = string(result->allocator, createInfo->objectDirectories[i]);
        }
        result->searchPaths[i] = result->sourceDirectories[i].buffer;
    }

    slang::CompilerOptionEntry compilerOptions[3];

    compilerOptions[0] = {};
    compilerOptions[0].name = slang::CompilerOptionName::Capability;
    compilerOptions[0].value.kind = slang::CompilerOptionValueKind::Int;
    compilerOptions[0].value.intValue0 = globalSession->findCapability("spvSparseResidency");
    
    compilerOptions[1] = {};
    compilerOptions[1].name = slang::CompilerOptionName::VulkanUseEntryPointName;
    compilerOptions[1].value.kind = slang::CompilerOptionValueKind::Int;
    compilerOptions[1].value.intValue0 = 1;

    compilerOptions[2] = {};
    compilerOptions[2].name = slang::CompilerOptionName::Optimization;
    compilerOptions[2].value.kind = slang::CompilerOptionValueKind::Int;
    compilerOptions[2].value.intValue0 = (SlangOptimizationLevel)createInfo->optimizationLevel;

    slang::SessionDesc desc = slang::SessionDesc();
    desc.compilerOptionEntryCount = 3;
    desc.compilerOptionEntries = compilerOptions;

    desc.searchPathCount = createInfo->numDirectories;
    if (createInfo->numDirectories > 0)
    {
        desc.searchPaths = result->searchPaths;
    }

    slang::TargetDesc targetDesc = slang::TargetDesc();
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = globalSession->findProfile("spirv_1_4");

    desc.targetCount = 1;
    desc.targets = &targetDesc;
    desc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

    slang::ISession *session;
    auto createSessionResult = globalSession->createSession(desc, &session);
    assert(createSessionResult == SLANG_OK);

    result->session = session;
    return result;
}
void ShaderCompiler_Deinit(ShaderCompiler *self)
{
    self->deinit();
    DEFAULT_FREE(self);
}

inline static void ShaderCompilationMeta_ParsePermutationArray(IAllocator allocator, const Json::JsonElement *elem, collections::Array<ShaderFunctionPermutation> &output)
{
    output = collections::Array<ShaderFunctionPermutation>(allocator, elem->arrayElements.length);
    for (u32 i = 0; i < elem->arrayElements.length; i++)
    {
        ShaderFunctionPermutation permutation = {};
        permutation.suffix = elem->arrayElements.data[i].key.Clone(allocator);
        const Json::JsonElement &member = elem->arrayElements.data[i].value;
        if (member.arrayElements.length > 0)
        {
            permutation.typeArguments = collections::Array<string>(allocator, member.arrayElements.length);
            for (u32 j = 0; j < member.arrayElements.length; j++)
            {
                permutation.typeArguments[j] = member.arrayElements.data[j].value.GetStringRaw(allocator);
            }
        }
        output[i] = permutation;
    }
}
ShaderCompilationMeta ShaderCompilationMeta_Parse(IAllocator allocator, Json::JsonElement *root)
{
    ShaderCompilationMeta result = {};

    Json::JsonElement *elem = root->GetProperty("vertexPermutations");
    if (elem == NULL)
    {
        elem = root->GetProperty("computePermutations");
    }
    if (elem != NULL)
    {
        ShaderCompilationMeta_ParsePermutationArray(allocator, elem, result.function1Permutations);
    }

    elem = root->GetProperty("fragmentPermutations");
    if (elem != NULL)
    {
        ShaderCompilationMeta_ParsePermutationArray(allocator, elem, result.function2Permutations);
    }
    return result;
}
ShaderCompilationMeta ShaderCompilationMeta_ParseFile(IAllocator allocator, text metaFilePath)
{
    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    string metaFileContents = io::ReadFile(arena.AsAllocator(), metaFilePath, false);
    Json::JsonElement rootElem = {};
    if (Json::ParseJsonDocument(arena.AsAllocator(), metaFileContents, &rootElem) > 0)
    {
        return {};
    }

    return ShaderCompilationMeta_Parse(allocator, &rootElem);
}

bool ShaderCompilerWriteBinaryFuncType(FILE *fs, slang::TypeLayoutReflection *typeLayout)
{
    slang::TypeReflection::Kind kind = typeLayout->getKind();
    switch (kind)
    {
        case slang::TypeReflection::Kind::ConstantBuffer:
        {
            Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_Uniform);
            Binary_WriteData<u32>(fs, (u32)typeLayout->getElementTypeLayout()->getSize());
            return true;
        }
        case slang::TypeReflection::Kind::SamplerState:
        {
            Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_Sampler);
            return true;
        }
        case slang::TypeReflection::Kind::Resource:
        {
            SlangResourceShape resourceType = typeLayout->getResourceShape();
            if (resourceType == SlangResourceShape::SLANG_STRUCTURED_BUFFER)
            {
                Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_StructuredBuffer);
            }
            else if (resourceType == SlangResourceShape::SLANG_TEXTURE_1D
            || resourceType == SlangResourceShape::SLANG_TEXTURE_2D
            || resourceType == SlangResourceShape::SLANG_TEXTURE_3D)
            {
                SlangResourceAccess accessMode = typeLayout->getResourceAccess();

                if (accessMode == SLANG_RESOURCE_ACCESS_WRITE || accessMode == SLANG_RESOURCE_ACCESS_READ_WRITE)
                {
                    Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_StorageTexture);
                }
                else
                {
                    Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_Texture);
                }
            }
            else
            {
                Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_Unknown);
                return false;
            }

            return true;
        }
        default:
        {
            Binary_WriteData<u32>(fs, (u32)ShaderCompilerResourceType_Unknown);
            return false;
        }
    }
}

typedef slang::VariableLayoutReflection *SlangVar;
typedef collections::List<SlangVar> SlangVarList;
void ShaderCompilerWriteBinaryFuncParams(FILE *fs, slang::ProgramLayout *layout)
{
    u32 paramCount = layout->getParameterCount();
    if (paramCount == 0)
    {
        Binary_WriteData<u32>(fs, 0);
        return;
    }
    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    IAllocator tempAlloc = arena.AsAllocator();

    Scope(ArenaAllocator, arena);

    collections::DenseSet<SlangVarList> setsToVars = collections::DenseSet<SlangVarList>(tempAlloc);
    u32 maxSetIndex = 0;
    for (u32 i = 0; i < paramCount; i++)
    {
        SlangVar var = layout->getParameterByIndex(i);
        u32 setIndex = var->getBindingSpace();
        if (setIndex > maxSetIndex)
        {
            maxSetIndex = setIndex;
        }
        SlangVarList *list = setsToVars.Get(setIndex);
        if (list == NULL || list->ptr == NULL)
        {
            list = setsToVars.Insert(setIndex, SlangVarList(tempAlloc));
        }
        list->Add(var);
    }

    maxSetIndex += 1;

    Binary_WriteData<u32>(fs, maxSetIndex);
    for (u32 i = 0; i < maxSetIndex; i++)
    {
        SlangVarList &list = setsToVars[i];

        Binary_WriteData<u32>(fs, list.count);

        for (u32 j = 0; j < list.count; j++)
        {
            SlangVar var = list[j];
            Binary_WriteText(fs, var->getName());
            Binary_WriteData<u32>(fs, var->getBindingIndex());

            slang::TypeLayoutReflection *typeLayout = var->getTypeLayout();
            slang::TypeReflection::Kind kind = typeLayout->getKind();

            if (kind == slang::TypeReflection::Kind::Array)
            {
                slang::TypeLayoutReflection *elemType = typeLayout->getElementTypeLayout();
                
                const usize elemCountUSize = typeLayout->getTotalArrayElementCount();
                const u32 elemCount = elemCountUSize == SLANG_UNBOUNDED_SIZE ? 0xFFFFFFFF : (u32)elemCountUSize;

                Binary_WriteData<u32>(fs, elemCount);
                ShaderCompilerWriteBinaryFuncType(fs, elemType);
            }
            else
            {
                Binary_WriteData<u32>(fs, 0);
                ShaderCompilerWriteBinaryFuncType(fs, typeLayout);
            }
        }
    }
}
void ShaderCompilerWriteBinaryFuncSpv(FILE *fs, ShaderCompilerShaderStage forStage, slang::IBlob *code)
{
    usize size = code->getBufferSize();
    Binary_WriteData<u32>(fs, (u32)forStage);
    Binary_WriteData<u32>(fs, (u32)size);

    const void *ptr = code->getBufferPointer();
    fwrite(ptr, 1, size, fs);
}
i32 ShaderCompilerWriteBinaryFunc(ShaderCompiler *self, FILE *fs, const OutputProgram &program, slang::IBlob *diagnostics)
{
    //version
    Binary_WriteData<u32>(fs, FUNC_BINARY_FILE_VERSION);
    //shader type
    //0: Vertex-Fragment
    //1: Compute
    bool isCompute = program.type == ShaderCompilerShaderStage_Compute;
    Binary_WriteData<u32>(fs, isCompute ? 1 : 0);

    //uniforms
    slang::ProgramLayout *layout = program.linkedProgram->getLayout();
    ShaderCompilerWriteBinaryFuncParams(fs, layout);

    if (isCompute)
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = program.linkedProgram->getTargetCode(0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteBinaryFuncSpv(fs, ShaderCompilerShaderStage_Compute, code);
        }
        code->Release();
    }
    else
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = program.linkedProgram->getEntryPointCode(0, 0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteBinaryFuncSpv(fs, ShaderCompilerShaderStage_Vertex, code);
        }
        code->Release();

        getEntryPointResult = program.linkedProgram->getEntryPointCode(1, 0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteBinaryFuncSpv(fs, ShaderCompilerShaderStage_Fragment, code);
        }
        code->Release();
    }
    return 0;
}

bool ShaderCompilerWriteJSONFuncType(Json::JsonWriter &writer, slang::TypeLayoutReflection *typeLayout)
{
    slang::TypeReflection::Kind kind = typeLayout->getKind();
    switch (kind)
    {
        case slang::TypeReflection::Kind::ConstantBuffer:
        {
            writer.WriteString("UniformBuffer");

            writer.WritePropertyName("size");
            writer.WriteUintValue(typeLayout->getElementTypeLayout()->getSize());
            return true;
        }
        case slang::TypeReflection::Kind::SamplerState:
        {
            writer.WriteString("Sampler");
            return true;
        }
        case slang::TypeReflection::Kind::Resource:
        {
            SlangResourceShape resourceType = typeLayout->getResourceShape();
            if (resourceType == SlangResourceShape::SLANG_STRUCTURED_BUFFER)
            {
                writer.WriteString("StorageBuffer");
            }
            else if (resourceType == SlangResourceShape::SLANG_TEXTURE_1D
            || resourceType == SlangResourceShape::SLANG_TEXTURE_2D
            || resourceType == SlangResourceShape::SLANG_TEXTURE_3D)
            {
                SlangResourceAccess accessMode = typeLayout->getResourceAccess();

                if (accessMode == SLANG_RESOURCE_ACCESS_WRITE || accessMode == SLANG_RESOURCE_ACCESS_READ_WRITE)
                {
                    writer.WriteString("StorageImage");
                }
                else
                {
                    writer.WriteString("Image");
                }
            }
            else
            {
                writer.WriteString("Unknown");
                return false;
            }

            return true;
        }
        default:
        {
            writer.WriteString("Unknown");
            return false;
        }
    }
}
void ShaderCompilerWriteJSONFuncParams(Json::JsonWriter &writer, slang::ProgramLayout *layout)
{
    u32 paramCount = layout->getParameterCount();

    for (u32 i = 0; i < paramCount; i++)
    {
        slang::VariableLayoutReflection *var = layout->getParameterByIndex(i);

        writer.WriteStartObject();

        writer.WritePropertyName("name");
        writer.WriteString(var->getName());

        writer.WritePropertyName("set");
        writer.WriteUintValue(var->getBindingSpace());

        writer.WritePropertyName("binding");
        writer.WriteUintValue(var->getBindingIndex());

        writer.WritePropertyName("type");
        slang::TypeLayoutReflection *typeLayout = var->getTypeLayout();
        slang::TypeReflection::Kind kind = typeLayout->getKind();
        if (kind == slang::TypeReflection::Kind::Array)
        {
            slang::TypeLayoutReflection *elemType = typeLayout->getElementTypeLayout();

            ShaderCompilerWriteJSONFuncType(writer, elemType);

            writer.WritePropertyName("arrayLength");
            writer.WriteUintValue(typeLayout->getTotalArrayElementCount());
        }
        else
        {
            ShaderCompilerWriteJSONFuncType(writer, typeLayout);
        }

        writer.WriteEndObject();
    }
}
void ShaderCompilerWriteJSONFuncSpv(Json::JsonWriter &writer, const char *propertyName, slang::IBlob *code)
{
    writer.WritePropertyName(propertyName);
    writer.WriteStartArray();
    usize size = code->getBufferSize();
    const void *ptr = code->getBufferPointer();
    usize sizeInt = size / 4;
    for (usize i = 0; i < sizeInt; i++)
    {
        writer.WriteUintValue(((const u32 *)ptr)[i]);
    }
    writer.WriteEndArray();
}
i32 ShaderCompilerWriteJSONFunc(ShaderCompiler *self, FILE *fs, const OutputProgram &program, slang::IBlob *diagnostics)
{
    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    //fwrite(code->getBufferPointer(), 1, code->getBufferSize(), fs);
    Json::JsonWriter writer = Json::JsonWriter(arena.AsAllocator(), fs, 4);

    bool isCompute = program.type == ShaderCompilerShaderStage_Compute;

    writer.WriteStartObject();

    writer.WritePropertyName("type");
    writer.WriteString(isCompute ? "Compute" : "Vertex-Fragment");
    writer.WritePropertyName("uniforms");
    writer.WriteStartArray();

    slang::ProgramLayout *layout = program.linkedProgram->getLayout();

    ShaderCompilerWriteJSONFuncParams(writer, layout);

    writer.WriteEndArray();

    if (isCompute)
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = program.linkedProgram->getTargetCode(0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteJSONFuncSpv(writer, "spv", code);
        }
        code->Release();
    }
    else
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = program.linkedProgram->getEntryPointCode(0, 0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteJSONFuncSpv(writer, "vertex", code);
        }
        code->Release();

        getEntryPointResult = program.linkedProgram->getEntryPointCode(1, 0, &code, &diagnostics);
        if (getEntryPointResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }
        else
        {
            ShaderCompilerWriteJSONFuncSpv(writer, "fragment", code);
        }
        code->Release();
    }

    writer.WriteEndObject();

    return 0;
}

slang::IComponentType *ShaderCompiler_Compile_SpecializeEntryPoint(const ShaderFunctionPermutation &permutation, slang::IEntryPoint *entryPoint, StringBuilder &errors)
{
    bool allocated = false;
    slang::IComponentType *result = NULL;
    u32 permutationArrayLength = permutation.typeArguments.length;

    slang::SpecializationArg argsArray[8];
    slang::SpecializationArg *args;
    
    if (permutationArrayLength > 8)
    {
        args = (slang::SpecializationArg *)DEFAULT_ALLOC(sizeof(slang::SpecializationArg) * permutationArrayLength);
        allocated = true;
    }
    else
    {
        args = argsArray;
    }

    for (u32 j = 0; j < permutationArrayLength; j++)
    {
        args[j] = slang::SpecializationArg::fromExpr(permutation.typeArguments.data[j].buffer);
    }

    ISlangBlob *diagnostics = NULL;
    auto slResult = entryPoint->specialize(args, permutationArrayLength, &result, &diagnostics);
    
    if (permutationArrayLength > 8)
    {
        DEFAULT_FREE(args);
    }
    if (slResult != SLANG_OK)
    {
        errors.AppendLine((text)diagnostics->getBufferPointer());
        diagnostics->Release();

        return NULL;
    }

    return result;
}
bool ShaderCompiler_Compile_SpecializeInto(
    slang::ISession *session, 
    StringBuilder &errors, 
    slang::IEntryPoint *entryPoint, 
    const collections::Array<ShaderFunctionPermutation> &permutations,
    slang::IComponentType **components, 
    bool isCompute,
    u32 outputComponentsIndex, 
    collections::List<OutputProgram> &outputPrograms,
    collections::List<slang::IComponentType *> &outputSpecializedEntryPoints)
{
    const u32 componentsCount = isCompute ? 2 : 3;
    const u32 totalPermutations = permutations.length;
    const u32 permutationArrayLength = permutations.data[0].typeArguments.length;
    bool errored = false;
    
    for (u32 i = 0; i < totalPermutations; i++)
    {
        components[outputComponentsIndex] = ShaderCompiler_Compile_SpecializeEntryPoint(permutations.data[i], entryPoint, errors);
        if (components[outputComponentsIndex] == NULL)
        {
            errored = true;
            continue;
        }

        outputSpecializedEntryPoints.Add(components[outputComponentsIndex]);
        
        OutputProgram outputProgram = {};
        outputProgram.type = isCompute ? ShaderCompilerShaderStage_Compute : ShaderCompilerShaderStage_Vertex;
        slang::IBlob *diagnostics = NULL;
        auto slResult = session->createCompositeComponentType(components, componentsCount, &outputProgram.program, &diagnostics);
        if (slResult != SLANG_OK)
        {
            errors.AppendLine((text)diagnostics->getBufferPointer());
            diagnostics->Release();
            errored = true;

            continue;
        }
        outputProgram.suffix = permutations.data[i].suffix.Clone(outputPrograms.allocator);
        outputPrograms.Add(outputProgram);
    }

    return !errored;
}

i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text overrideOutputPath, i32 useSourceDirectoryOfIndex)
{
    i32 sourceDirectoryIndex = useSourceDirectoryOfIndex;

    if (sourceDirectoryIndex < 0 || sourceDirectoryIndex >= self->numDirectories)
    {
        char *maxPath = (char *)DEFAULT_ALLOC(MAX_PATH);
        memset(maxPath, 0, MAX_PATH);
        for (u32 i = 0; i < self->numDirectories; i++)
        {
            //i32 length = 
            snprintf(maxPath, MAX_PATH, "%s/%s", self->sourceDirectories[i].buffer, filePathRelative);
            if (io::FileExists(maxPath))
            {
                sourceDirectoryIndex = i;
                break;
            }
        }
    }
    if (sourceDirectoryIndex < 0 || sourceDirectoryIndex >= self->numDirectories)
    {
        self->errors.Appendf("Could not find a file with the path %s relative to any of the provided sourceDirectories.\n", filePathRelative);
        if (io::FileExists(filePathRelative))
        {
            self->errors.AppendLine(" - It appears that a file of that path does exist, but the path is not relative to any of the provided sourceDirectories, or was not able to be accessed for some reason.");
        }
        return 1;
    }

    ShaderCompilationMeta fileMeta = {};
    Scope(ShaderCompilationMeta, fileMeta);
    string metaFilePath = string::Format(GetCAllocator(), "%s/%s.vars", self->sourceDirectories[sourceDirectoryIndex].buffer, filePathRelative);
    if (io::FileExists(metaFilePath.buffer))
    {
        fileMeta = ShaderCompilationMeta_ParseFile(GetCAllocator(), metaFilePath.buffer);
    }
    metaFilePath.deinit();

    CharSlice filePathRelativeSlice = CharSlice(filePathRelative);

    slang::ISession *session = self->session;
    bool errored = false;
    slang::IBlob *diagnostics = NULL;
    slang::IModule *module = session->loadModule(filePathRelative, &diagnostics);

    errored = module == NULL;

    if (errored)
    {
        if (diagnostics)
        {
            self->errors.AppendLine((text)diagnostics->getBufferPointer());
            diagnostics->Release();
            diagnostics = NULL;
        }
        else
        {
            self->errors.AppendLine("Unknown error encountered");
        }
        return 1;
    }
    
    if (self->objectDirectories != NULL && filePathRelativeSlice.EndsWith(".inc.slang"))
    {
        const string objDir = self->objectDirectories[sourceDirectoryIndex];

        //module file
        string objPath = string::Format(GetCAllocator(), "%s/%s", objDir, filePathRelative);
        io::RecursiveCreateDirectories(path::GetDirectory(objPath));
        path::SwapExtensionDeinit(objPath, ".slang-module");
        module->writeToFile(objPath.buffer);
        return 0;
    }

    LoadedModule loaded = {};
    loaded.specializedEntryPoints = collections::List<slang::IComponentType *>(GetCAllocator());
    loaded.module = module;

    if (module->findEntryPointByName("VertexFunction", &loaded.entryPoint1) == SLANG_OK)
    {
        if (module->findEntryPointByName("FragmentFunction", &loaded.entryPoint2) != SLANG_OK)
        {
            self->errors.Append("Detected file %s to be a vertex-fragment shader, but could not find a valid function 'FragmentFunction' as the entry point\n");
            errored = true;
        }
    }
    else if (module->findEntryPointByName("main", &loaded.entryPoint1) != SLANG_OK)
    {
        self->errors.Appendf("Could not discern what type of shader %s is\n", filePathRelative);
        errored = true;
    }

    collections::List<OutputProgram> programs = collections::List<OutputProgram>(GetCAllocator());

    if (!errored)
    {
        const u32 vertexSpecializations = loaded.entryPoint1->getSpecializationParamCount();
        if (loaded.entryPoint2 != NULL)
        {
            const u32 fragmentSpecializations = loaded.entryPoint2->getSpecializationParamCount();
            if (vertexSpecializations > 0)
            {
                if (fileMeta.function1Permutations.length == 0)
                {
                    self->errors.AppendLine("Vertex shader function expected at least one permutation, but none were provided.");
                    return 1;
                }
                if (fileMeta.function1Permutations[0].typeArguments.length != vertexSpecializations)
                {
                    self->errors.Appendf("Vertex shader function expected a permutation consisting of %u types, but a permutation consisting of %u types was provided instead.\n", vertexSpecializations, (u32)fileMeta.function1Permutations[0].typeArguments.length);
                    return 1;
                }
            }
            if (fragmentSpecializations > 0)
            {
                if (fileMeta.function2Permutations.length == 0)
                {
                    self->errors.AppendLine("Fragment shader function expected at least one permutation, but none were provided.");
                    return 1;
                }
                if (fileMeta.function2Permutations[0].typeArguments.length != fragmentSpecializations)
                {
                    self->errors.Appendf("Fragment shader function expected a permutation consisting of %u types, but a permutation consisting of %u types was provided instead.\n", fragmentSpecializations, (u32)fileMeta.function2Permutations[0].typeArguments.length);
                    return 1;
                }
            }
            
            if (vertexSpecializations == 0 && fragmentSpecializations == 0)
            {
                OutputProgram outputProgram = {};
                outputProgram.type = ShaderCompilerShaderStage_Vertex;
                slang::IComponentType *components[] = {loaded.module, loaded.entryPoint1, loaded.entryPoint2};
                auto slResult = session->createCompositeComponentType(components, 3, &outputProgram.program, &diagnostics);
                if (slResult != SLANG_OK)
                {
                    errored = true;
                }
                else programs.Add(outputProgram);
            }
            else if (vertexSpecializations > 0 && fragmentSpecializations == 0)
            {
                slang::IComponentType *components[] = {loaded.module, NULL, loaded.entryPoint2};
            
                if (!ShaderCompiler_Compile_SpecializeInto(
                    session, 
                    self->errors,
                    loaded.entryPoint1,
                    fileMeta.function1Permutations,
                    components,
                    false,
                    1,
                    programs,
                    loaded.specializedEntryPoints))
                {
                    errored = true;
                }
            }
            else if (fragmentSpecializations > 0 && vertexSpecializations == 0)
            {
                slang::IComponentType *components[] = {loaded.module, loaded.entryPoint1, NULL};
            
                if (!ShaderCompiler_Compile_SpecializeInto(
                    session, 
                    self->errors,
                    loaded.entryPoint2,
                    fileMeta.function2Permutations,
                    components,
                    false,
                    2,
                    programs,
                    loaded.specializedEntryPoints))
                {
                    errored = true;
                }
            }
            else
            {
                //each member of vertexSpecializations must specialize with each member
                //of fragmentSpecializations.
                //This effectively results in vertex * fragment amount of specializations.

                for (u32 i = 0; i < fileMeta.function1Permutations.length; i++)
                {
                    for (u32 j = 0; j < fileMeta.function2Permutations.length; j++)
                    {
                        slang::IComponentType *components[] = {loaded.module, NULL, NULL};

                        components[1] = ShaderCompiler_Compile_SpecializeEntryPoint(fileMeta.function1Permutations[i], loaded.entryPoint1, self->errors);
                        components[2] = ShaderCompiler_Compile_SpecializeEntryPoint(fileMeta.function2Permutations[j], loaded.entryPoint2, self->errors);
                    
                        if (components[1] == NULL || components[2] == NULL)
                        {
                            errored = true;
                            continue;
                        }

                        OutputProgram outputProgram = {};
                        auto slResult = session->createCompositeComponentType(components, 3, &outputProgram.program, &diagnostics);
                        if (slResult != SLANG_OK)
                        {
                            self->errors.AppendLine((text)diagnostics->getBufferPointer());
                            diagnostics->Release();
                            //set to null to avoid reporting twice
                            diagnostics = NULL;
                            errored = true;
                        }
                        else 
                        {
                            outputProgram.suffix = string::Format(programs.allocator, "%s%s", fileMeta.function1Permutations[i].suffix.buffer, fileMeta.function2Permutations[j].suffix.buffer);
                            programs.Add(outputProgram);
                        }
                    }
                }
            }
        }
        else
        {
            const u32 computeSpecializations = vertexSpecializations;
            if (computeSpecializations == 0)
            {
                OutputProgram outputProgram = {};
                outputProgram.type = ShaderCompilerShaderStage_Compute;
                slang::IComponentType *components[] = {loaded.module, loaded.entryPoint1 };
                auto slResult = session->createCompositeComponentType(components, 2, &outputProgram.program, &diagnostics);
                if (slResult != SLANG_OK)
                {
                    errored = true;
                }
                else programs.Add(outputProgram);
            }
            else
            {
                slang::IComponentType *components[] = {loaded.module, NULL};
            
                if (!ShaderCompiler_Compile_SpecializeInto(
                    session, 
                    self->errors,
                    loaded.entryPoint1,
                    fileMeta.function1Permutations,
                    components,
                    true,
                    1,
                    programs,
                    loaded.specializedEntryPoints))
                {
                    diagnostics = NULL;
                    errored = true;
                }
            }
        }
        if (diagnostics && errored)
        {
            self->errors.AppendLine((text)diagnostics->getBufferPointer());
            diagnostics->Release();
        }
    }

    //get the final output path without the .sfn
    string outputPath = string();
    Scope(string, outputPath);

    if (overrideOutputPath != NULL)
    {
        CharSlice outputPathSlice = CharSlice(overrideOutputPath);
        
        if (path::GetExtension(outputPathSlice).length > 0)
        {
            outputPath = path::SwapExtension(GetCAllocator(), overrideOutputPath, CharSlice());
        }
        else
        {
            outputPath = string(GetCAllocator(), overrideOutputPath);
        }
    }
    else
    {
        string nameSwap = path::SwapExtension(GetCAllocator(), filePathRelative, CharSlice());
        outputPath = string::Format(GetCAllocator(), "%s/%s", self->outputDirectories[sourceDirectoryIndex].buffer, nameSwap.buffer);
        nameSwap.deinit();
    }

    if (!errored)
    {
        i32 finalErrorCode = 0;
        for (u32 i = 0; i < programs.count; i++)
        {
            SlangResult linkResult = programs[i].program->link(&programs[i].linkedProgram, &diagnostics);
            if (linkResult != SLANG_OK)
            {
                if (diagnostics)
                {
                    finalErrorCode = 1;
                    self->errors.AppendLine((text)diagnostics->getBufferPointer());
                    diagnostics->Release();
                }
                continue;
            }
            assert(programs[i].linkedProgram != NULL);
            
            const bool isSFN = true;

            string fullOutputPath;
            if (programs[i].suffix.length > 1)
            {
                fullOutputPath = string::Format(GetCAllocator(), "%s%s.sfn", outputPath.buffer, programs[i].suffix.buffer);
            }
            else
            {
                fullOutputPath = string::Format(GetCAllocator(), "%s.sfn", outputPath.buffer);
            }

            FILE *fs = fopen(fullOutputPath.buffer, isSFN ? "wb" : "w");

            if (fs != NULL)
            {
                i32 errorCode;
                if (isSFN)
                {
                    errorCode = ShaderCompilerWriteBinaryFunc(self, fs, programs[i], diagnostics);
                }
                else
                {
                    errorCode = ShaderCompilerWriteJSONFunc(self, fs, programs[i], diagnostics);
                }
                
                if (errorCode != 0)
                {
                    finalErrorCode = errorCode;
                }
                fclose(fs);
            }
            else
            {
                self->errors.Appendf("Could not save file %s\n", fullOutputPath.buffer);
                finalErrorCode = 1;
            }
            fullOutputPath.deinit();
        }

        return finalErrorCode;
    }
    else
    {
        return 1;
    }
}
text ShaderCompiler_GetErrorMessages(ShaderCompiler *self)
{
    self->errorsString.deinit();
    self->errorsString = self->errors.ToString(GetCAllocator());

    return self->errorsString.buffer;
}