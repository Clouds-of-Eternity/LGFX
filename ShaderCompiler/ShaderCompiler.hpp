#pragma once
#include "slang/slang.h"
#include "string.hpp"
#include "stb_sprintf.h"
#include "StringBuilder.hpp"
#include "vector.hpp"
#include "ArenaAllocator.hpp"
#include "io.hpp"
#include "path.hpp"

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

void AssetcShaderCompilerInitialize();
void AssetcShaderCompilerUnload();

BeginExports()
ShaderCompiler *ShaderCompiler_New(text *includeDirectories, u32 includeDirectoriesCount);
void ShaderCompiler_Deinit(ShaderCompiler *self);
i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text outputPath);
text ShaderCompiler_GetErrorMessages(ShaderCompiler *self);