#include "ShaderCompiler.hpp"
#include "Json.hpp"
#include "scope.hpp"

slang::IGlobalSession *globalSession;
void AssetcShaderCompilerInitialize()
{
    auto createSessionResult = slang::createGlobalSession(&globalSession);
    if (createSessionResult != SLANG_OK)
    {
        globalSession = NULL;
    }
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
bool WriteType(Json::JsonWriter &writer, slang::TypeLayoutReflection *typeLayout)
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
void ShaderCompiler_Deinit(ShaderCompiler *self)
{
    self->deinit();
    free(self);
}

void WriteSpv(Json::JsonWriter &writer, const char *propertyName, slang::IBlob *code)
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

void WriteParams(Json::JsonWriter &writer, slang::ProgramLayout *layout)
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

            WriteType(writer, elemType);

            writer.WritePropertyName("arrayLength");
            writer.WriteUintValue(typeLayout->getTotalArrayElementCount());
        }
        else
        {
            WriteType(writer, typeLayout);
        }

        writer.WriteEndObject();
    }
}

i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text outputPath)
{
    slang::ISession *session = self->session;
    bool errored = false;
    slang::IBlob *diagnostics = NULL;
    slang::IModule *module = session->loadModule(filePathRelative, &diagnostics);

    errored = module == NULL;

    if (errored && diagnostics)
    {
        self->errors.AppendLine((text)diagnostics->getBufferPointer());
        diagnostics->Release();
        diagnostics = NULL;
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

        FILE *fs = fopen(outputPath, "w");
        if (fs != NULL)
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

            WriteParams(writer, layout);

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
                    //fprintf(stderr, "%s\n", (text)diagnostics->getBufferPointer());
                }
                else
                {
                    WriteSpv(writer, "spv", code);
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
                    //fprintf(stderr, "%s\n", (text)diagnostics->getBufferPointer());
                }
                else
                {
                    WriteSpv(writer, "vertex", code);
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
                    //fprintf(stderr, "%s\n", (text)diagnostics->getBufferPointer());
                }
                else
                {
                    WriteSpv(writer, "fragment", code);
                }
                code->Release();
            }

            writer.WriteEndObject();

            fclose(fs);
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