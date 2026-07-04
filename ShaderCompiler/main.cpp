#include "ShaderCompiler.hpp"
#include "io.hpp"
#include <stdio.h>
#include "Scope.hpp"
#include "Path.hpp"

i32 main(i32 argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "ShaderCompiler: Insufficient arguments\n");
        return 1;
    }

    if (CharSlice(argv[1]) == "--out-spv")
    {
        if (argc != 4)
        {
            fprintf(stderr, "ShaderCompiler: --out-spv requires two arguments thereafter: (input-file-path) and (output-file-path)");
            return 1;
        }
        if (!io::FileExists(argv[2]))
        {
            fprintf(stderr, "ShaderCompiler: Input file %s does not exist\n", argv[2]);
            return 1;
        }
        return ShaderCompiler_ExtractSpirvFromSFNFilePath(argv[2], argv[3]);
    }
    //argv[0] = exe path
    //argv[1] = compile all files in folder
    //argv[2] = include directories

    ShaderCompilerOptimizationLevel optLevel = ShaderCompilerOptimizationLevel_Default;

    u32 optArgsCount = argc - 2;
    collections::List<char *> includeDirectories = collections::List<char *>(GetCAllocator());
    Scope(collections::List<char *>, includeDirectories);
    includeDirectories.Add(argv[1]);
    bool invalidArgs = false;
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
        else
        {
            fprintf(stderr, "ShaderCompiler: Unknown argument %s\n", argString.buffer);
            invalidArgs = true;
        }
    }
    if (invalidArgs)
    {
        return 1;
    }

    AssetcShaderCompilerInitialize();
    ShaderCompilerCreateInfo createInfo = {};
    createInfo.numDirectories = includeDirectories.count;
    createInfo.sourceDirectories = (text*)includeDirectories.ptr;
    createInfo.outputDirectories = (text*)includeDirectories.ptr;
    createInfo.optimizationLevel = optLevel;

    ShaderCompiler *result = ShaderCompiler_Create(&createInfo);

    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    collections::Array<string> allFilePaths = io::GetFilesInDirectoryRecursive(arena.AsAllocator(), argv[1]);

    i32 returnCode;
    for (u32 i = 0; i < allFilePaths.length; i++)
    {
        if (allFilePaths[i].EndsWith(".slang") && !allFilePaths[i].EndsWith(".inc.slang"))
        {
            string relative = allFilePaths[i].CloneTrimStart(arena.AsAllocator(), strlen(argv[1]) + 1);
            string outputPath = path::SwapExtension(arena.AsAllocator(), allFilePaths[i], ".sfn");
            printf("ShaderCompiler: Compiling %s\n", relative.buffer);
            returnCode = ShaderCompiler_Compile(result, relative.buffer, outputPath.buffer, -1);
            if (returnCode != 0)
            {
                text error = ShaderCompiler_GetErrorMessages(result);
                fprintf(stderr, "ShaderCompiler: %s\n", error);
                break;
            }
        }
    }

    ShaderCompiler_Deinit(result);
    AssetcShaderCompilerUnload();

    return returnCode;
}