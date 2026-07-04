#pragma once
#include "slang/slang.h"
#include "stb_sprintf.h"
#include "StringBuilder.hpp"
#include "List.hpp"
#include "ArenaAllocator.hpp"
#include "io.hpp"
#include "DataStream.hpp"
#include "HashSet.hpp"

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

struct OutputProgram
{
    string suffix;
    slang::IComponentType *program;
    slang::IComponentType *linkedProgram;
    ShaderCompilerShaderStage type;

    inline void deinit()
    {
        program->Release();
        linkedProgram->Release();
    }
};
struct LoadedModule
{
    slang::IModule *module;
    slang::IEntryPoint *entryPoint1;
    slang::IEntryPoint *entryPoint2;

    collections::List<slang::IComponentType *> specializedEntryPoints;

    inline void deinit()
    {
        module->Release();
        entryPoint1->Release();
        entryPoint2->Release();

        for (u32 i = 0; i < specializedEntryPoints.count; i++)
        {
            specializedEntryPoints[i]->Release();
        }
        specializedEntryPoints.deinit();
    }
};
struct ShaderCompiler
{
    IAllocator allocator;
    slang::ISession *session;
    StringBuilder errors;
    string errorsString;

    text *searchPaths;

    u32 numDirectories;
    string *sourceDirectories;
    string *outputDirectories;
    string *objectDirectories;

    collections::HashSet<string> allParsedShadersVariants;

    inline ShaderCompiler()
    {
        allocator = {};
        errors = StringBuilder();
        errorsString = string();
        session = NULL;

        numDirectories = 0;
        sourceDirectories = NULL;
        outputDirectories = NULL;
        objectDirectories = NULL; 
        searchPaths = NULL;

        allParsedShadersVariants = collections::HashSet<string>();
    }
    inline ShaderCompiler(IAllocator allocator, u32 numDirectories, bool storeObjects)
    {
        this->allocator = allocator;
        errors = StringBuilder(allocator);
        errorsString = string();
        session = NULL;

        this->numDirectories = numDirectories;
        sourceDirectories = allocator.AllocateInstancesOf<string>(numDirectories);
        outputDirectories = allocator.AllocateInstancesOf<string>(numDirectories);
        if (storeObjects)
        {
            objectDirectories = allocator.AllocateInstancesOf<string>(numDirectories);
        }
        else objectDirectories = NULL;
        searchPaths = allocator.AllocateInstancesOf<text>(numDirectories);

        allParsedShadersVariants = collections::HashSet<string>(allocator, &stringHash, &stringEql);
    }
    inline void deinit()
    {
        session->Release();
        errors.deinit();
        errorsString.deinit();

        if (searchPaths != NULL)
        {
            allocator.Free(searchPaths);
        }
        if (numDirectories > 0)
        {
            for (u32 i = 0; i < numDirectories; i++)
            {
                sourceDirectories[i].deinit();
                outputDirectories[i].deinit();
                if (objectDirectories != NULL)
                    objectDirectories[i].deinit();
            }
            allocator.Free(sourceDirectories);
            allocator.Free(outputDirectories);
            if (objectDirectories != NULL)
                allocator.Free(objectDirectories);
        }

        auto iterator = allParsedShadersVariants.GetIterator();
        foreach (val, iterator)
        {
            val->deinit();
        }
        allParsedShadersVariants.deinit();
    }
};

struct ShaderCompilerCreateInfo
{
    text *sourceDirectories;
    text *outputDirectories;
    text *objectDirectories;

    ShaderCompilerOptimizationLevel optimizationLevel;
    uint32_t numDirectories;
};

struct ShaderFunctionPermutation
{
    string suffix;
    collections::Array<string> typeArguments;

    inline void deinit()
    {
        suffix.deinit();
        for (u32 i = 0; i < typeArguments.length; i++)
        {
            typeArguments[i].deinit();
        }
        typeArguments.deinit();
    }
};
struct ShaderCompilationMeta
{
    string sourceFilePath;
    collections::Array<ShaderFunctionPermutation> function1Permutations;
    collections::Array<ShaderFunctionPermutation> function2Permutations;

    inline void deinit()
    {
        if (sourceFilePath.buffer != NULL)
        {
            sourceFilePath.deinit();
        }
        for (u32 i = 0; i < function1Permutations.length; i++)
        {
            function1Permutations[i].deinit();
        }
        function1Permutations.deinit();

        if (function2Permutations.length > 0)
        {
            for (u32 i = 0; i < function2Permutations.length; i++)
            {
                function2Permutations[i].deinit();
            }
            function2Permutations.deinit();
        }
    }
};

bool AssetcShaderCompilerInitialize();
void AssetcShaderCompilerUnload();

BeginExports()
usize ShaderCompiler_ExtractSpirvFromSFNFilePath(text name, text outputPathNoFileExtension);
usize ShaderCompiler_ExtractSpirvFromSFN(IDataStream input, text outputPathNoFileExtension);
ShaderCompiler *ShaderCompiler_Create(const ShaderCompilerCreateInfo *createInfo);
void ShaderCompiler_Deinit(ShaderCompiler *self);
i32 ShaderCompiler_CompileVariant(ShaderCompiler *self, text metaFilePath, text overrideOutputPath);
i32 ShaderCompiler_Compile(ShaderCompiler *self, text filePathRelative, text overrideOutputPath, const ShaderCompilationMeta *overrideCompilationMeta, i32 useSourceDirectoryOfIndex);
text ShaderCompiler_GetErrorMessages(ShaderCompiler *self);