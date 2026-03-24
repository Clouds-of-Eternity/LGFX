#include "ShaderCompiler.hpp"
#include "Json.hpp"
#include "DenseSet.hpp"
#include "BinaryIO.hpp"
#include "Scope.hpp"

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
ShaderCompiler *ShaderCompiler_New(text *includeDirectories, u32 includeDirectoriesCount, ShaderCompilerOptimizationLevel optimizationLevel)
{
    assert(globalSession != NULL);
    ShaderCompiler *result = (ShaderCompiler *)malloc(sizeof(ShaderCompiler));
    *result = ShaderCompiler(GetCAllocator());

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
    compilerOptions[2].value.intValue0 = (SlangOptimizationLevel)optimizationLevel;

    slang::SessionDesc desc = slang::SessionDesc();
    desc.compilerOptionEntryCount = 3;
    desc.compilerOptionEntries = compilerOptions;

    desc.searchPathCount = includeDirectoriesCount;
    if (includeDirectoriesCount > 0)
    {
        desc.searchPaths = includeDirectories;
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
    free(self);
}

// enum ShaderVariableType
// {

// };

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
        if (list == NULL)
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
            Binary_WriteData<u32>(fs, var->getBindingSpace());
            Binary_WriteData<u32>(fs, var->getBindingIndex());

            slang::TypeLayoutReflection *typeLayout = var->getTypeLayout();
            slang::TypeReflection::Kind kind = typeLayout->getKind();

            if (kind == slang::TypeReflection::Kind::Array)
            {
                slang::TypeLayoutReflection *elemType = typeLayout->getElementTypeLayout();

                Binary_WriteData<u32>(fs, (u32)typeLayout->getTotalArrayElementCount());
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
    printf("Wrote size %llu\n", size);
    Binary_WriteData<u32>(fs, (u32)forStage);
    Binary_WriteData<u32>(fs, (u32)size);

    const void *ptr = code->getBufferPointer();
    fwrite(ptr, 1, size, fs);
}
i32 ShaderCompilerWriteBinaryFunc(ShaderCompiler *self, FILE *fs, LoadedModule &loaded, slang::IBlob *diagnostics)
{
    //version
    Binary_WriteData<u32>(fs, FUNC_BINARY_FILE_VERSION);
    //shader type
    //0: Vertex-Fragment
    //1: Compute
    Binary_WriteData<u32>(fs, loaded.entryPoint2 != NULL ? 0 : 1);

    //uniforms
    slang::ProgramLayout *layout = loaded.linkedProgram->getLayout();
    ShaderCompilerWriteBinaryFuncParams(fs, layout);

    if (loaded.entryPoint2 == NULL)
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = loaded.linkedProgram->getTargetCode(0, &code, &diagnostics);
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
        SlangResult getEntryPointResult = loaded.linkedProgram->getEntryPointCode(0, 0, &code, &diagnostics);
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

        getEntryPointResult = loaded.linkedProgram->getEntryPointCode(1, 0, &code, &diagnostics);
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
i32 ShaderCompilerWriteJSONFunc(ShaderCompiler *self, FILE *fs, LoadedModule &loaded, slang::IBlob *diagnostics)
{
    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    //fwrite(code->getBufferPointer(), 1, code->getBufferSize(), fs);
    Json::JsonWriter writer = Json::JsonWriter(arena.AsAllocator(), fs, true);

    writer.WriteStartObject();

    writer.WritePropertyName("type");
    writer.WriteString(loaded.entryPoint2 != NULL ? "Vertex-Fragment" : "Compute");
    writer.WritePropertyName("uniforms");
    writer.WriteStartArray();

    slang::ProgramLayout *layout = loaded.linkedProgram->getLayout();

    ShaderCompilerWriteJSONFuncParams(writer, layout);

    writer.WriteEndArray();

    if (loaded.entryPoint2 == NULL)
    {
        slang::IBlob *code = NULL;
        SlangResult getEntryPointResult = loaded.linkedProgram->getTargetCode(0, &code, &diagnostics);
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
        SlangResult getEntryPointResult = loaded.linkedProgram->getEntryPointCode(0, 0, &code, &diagnostics);
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

        getEntryPointResult = loaded.linkedProgram->getEntryPointCode(1, 0, &code, &diagnostics);
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

i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text outputPath)
{
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
    LoadedModule loaded = {};
    loaded.module = module;

    if (module->findEntryPointByName("VertexFunction", &loaded.entryPoint1) == SLANG_OK)
    {
        if (module->findEntryPointByName("FragmentFunction", &loaded.entryPoint2) != SLANG_OK)
        {
            self->errors.Append("Detected file %s to be a vertex-fragment shader, but could not find a valid function 'FragmentFunction' as the entry point\n");
            //fprintf(stderr, "Detected file %s to be a vertex-fragment shader, but could not find a valid function 'FragmentFunction' as the entry point\n", filePathRelative);
            errored = true;
        }
    }
    else if (module->findEntryPointByName("main", &loaded.entryPoint1) != SLANG_OK)
    {
        //fprintf(stderr, "Could not discern what type of shader %s is\n", filePathRelative);

        self->errors.Appendf("Could not discern what type of shader %s is\n", filePathRelative);
        errored = true;
    }

    if (!errored)
    {
        if (loaded.entryPoint2 != NULL)
        {
            slang::IComponentType *components[] = {loaded.module, loaded.entryPoint1, loaded.entryPoint2};
            auto slResult = session->createCompositeComponentType(components, 3, &loaded.program, &diagnostics);
            if (slResult != SLANG_OK)
            {
                errored = true;
            }
        }
        else
        {
            slang::IComponentType *components[] = {loaded.module, loaded.entryPoint1};
            auto slResult = session->createCompositeComponentType(components, 2, &loaded.program, &diagnostics);
            if (slResult != SLANG_OK)
            {
                errored = true;
            }
        }
        if (diagnostics && errored)
        {
            self->errors.AppendLine((text)diagnostics->getBufferPointer());
            diagnostics->Release();
        }
    }

    if (!errored)
    {
        SlangResult linkResult = loaded.program->link(&loaded.linkedProgram, &diagnostics);
        if (linkResult != SLANG_OK)
        {
            if (diagnostics)
            {
                self->errors.AppendLine((text)diagnostics->getBufferPointer());
                diagnostics->Release();
            }
            return 1;
        }

        CharSlice outputPathSlice = CharSlice(outputPath);
        bool isSFN = outputPathSlice.EndsWith(".sfn");

        FILE *fs = fopen(outputPath, isSFN ? "wb" : "w");
        if (fs != NULL)
        {

            i32 errorCode;
            if (isSFN)
            {
                errorCode = ShaderCompilerWriteBinaryFunc(self, fs, loaded, diagnostics);
            }
            else
            {
                errorCode = ShaderCompilerWriteJSONFunc(self, fs, loaded, diagnostics);
            }
            
            fclose(fs);
            return errorCode;
        }
        return 0;
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