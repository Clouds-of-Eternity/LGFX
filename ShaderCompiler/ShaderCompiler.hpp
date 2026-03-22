#pragma once
#include "slang/slang.h"
#include "stb_sprintf.h"
#include "StringBuilder.hpp"
#include "List.hpp"
#include "ArenaAllocator.hpp"
#include "io.hpp"
#include "Path.hpp"

#define FUNC_BINARY_FILE_VERSION 1

enum ShaderCompilerOptimizationLevel : i32
{
    ShaderCompilerOptimizationLevel_None,
    ShaderCompilerOptimizationLevel_Default,
    ShaderCompilerOptimizationLevel_High,
    ShaderCompilerOptimizationLevel_Maximum
};
enum ShaderCompilerResourceType
{
    ShaderCompilerResourceType_Uniform,
    ShaderCompilerResourceType_Sampler,
    ShaderCompilerResourceType_Texture,
    ShaderCompilerResourceType_StructuredBuffer,
    ShaderCompilerResourceType_InputAttachment,
    ShaderCompilerResourceType_StorageTexture,
    ShaderCompilerResourceType_Unknown = 0xFFFFFFFF
};
enum ShaderCompilerShaderStage
{
    ShaderCompilerShaderStage_Vertex,
    ShaderCompilerShaderStage_Fragment,
    ShaderCompilerShaderStage_Compute
};

struct LoadedModule
{
    slang::IModule *module;
    slang::IEntryPoint *entryPoint1;
    slang::IEntryPoint *entryPoint2;
    slang::IComponentType *program;
    slang::IComponentType *linkedProgram;

    inline void deinit()
    {
        module->Release();
        entryPoint1->Release();
        entryPoint2->Release();
        program->Release();
        linkedProgram->Release();
    }
};
struct ShaderCompiler
{
    slang::ISession *session;
    StringBuilder errors;
    string errorsString;

    inline ShaderCompiler()
    {
        errors = StringBuilder();
        errorsString = string();
        session = NULL;
    }
    inline ShaderCompiler(IAllocator allocator)
    {
        errors = StringBuilder(allocator);
        errorsString = string();
        session = NULL;
    }
    inline void deinit()
    {
        session->Release();
        errors.deinit();
        errorsString.deinit();
    }
};

bool AssetcShaderCompilerInitialize();
void AssetcShaderCompilerUnload();

BeginExports()
ShaderCompiler *ShaderCompiler_New(text *includeDirectories, u32 includeDirectoriesCount, ShaderCompilerOptimizationLevel optimizationLevel);
void ShaderCompiler_Deinit(ShaderCompiler *self);
i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text outputPath);
text ShaderCompiler_GetErrorMessages(ShaderCompiler *self);