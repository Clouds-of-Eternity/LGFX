#include "ShaderCompiler.hpp"
#include "io.hpp"
#include <stdio.h>
#include "scope.hpp"

i32 main(i32 argc, char **argv)
{
    if (argc < 2)
    {
        return 1;
    }
    //argv[0] = exe path
    //argv[1] = compile all files in folder
    //argv[2] = include directories

    ShaderCompilerOptimizationLevel optLevel = ShaderCompilerOptimizationLevel_Default;

    u32 optArgsCount = argc - 2;
    collections::vector<char *> includeDirectories = collections::vector<char *>(GetCAllocator());
    Scope(collections::vector<char *>, includeDirectories);
    for (u32 i = 0; i < optArgsCount; i++)
    {
        u32 index = i + 2;
        string argString;
        argString.buffer = argv[index];
        argString.length = strlen(argv[index]) + 1;
        if (argString.StartsWith("-i"))
        {
            includeDirectories.Add(&argString.buffer[2]);
            printf("-Including directory %s\n", &argString.buffer[2]);
        }
        else if (argString.StartsWith("-O"))
        {
            if (argString == "-O0")
            {
                optLevel = ShaderCompilerOptimizationLevel_None;
                printf("-No optimizations will be applied\n");
            }
            else if (argString == "-O1")
            {
                optLevel = ShaderCompilerOptimizationLevel_Default;
                printf("-Some optimizations will be applied\n");
            }
            else if (argString == "-O2")
            {
                optLevel = ShaderCompilerOptimizationLevel_High;
                printf("-Most optimizations will be applied\n");
            }
            else if (argString == "-O3")
            {
                optLevel = ShaderCompilerOptimizationLevel_Maximum;
                printf("-All optimizations will be applied\n");
            }
        }
    }

    AssetcShaderCompilerInitialize();
    ShaderCompiler *result = ShaderCompiler_New((text*)includeDirectories.ptr, includeDirectories.count, optLevel);

    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    collections::Array<string> allFilePaths = io::GetFilesInDirectoryRecursive(arena.AsAllocator(), argv[1]);

    i32 returnCode;
    for (u32 i = 0; i < allFilePaths.length; i++)
    {
        if (allFilePaths[i].EndsWith(".slang") && !allFilePaths[i].EndsWith(".inc.slang"))
        {
            string relative = allFilePaths[i].CloneTrimStart(arena.AsAllocator(), strlen(argv[1]) + 1);
            string outputPath = path::SwapExtension(arena.AsAllocator(), allFilePaths[i], ".func");
            printf("Compiling %s\n", relative.buffer);
            returnCode = ShaderCompiler_Compile(result, allFilePaths[i].buffer, outputPath.buffer);
            if (returnCode != 0)
            {
                text error = ShaderCompiler_GetErrorMessages(result);
                fprintf(stderr, "%s\n", error);
                break;
            }
        }
    }

    ShaderCompiler_Deinit(result);
    AssetcShaderCompilerUnload();

    return returnCode;
}