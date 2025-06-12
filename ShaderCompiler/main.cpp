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
    u32 includesCount = argc - 2;
    text *includeDirectories = NULL;
    if (includesCount > 0)
    {
        includeDirectories = (text *)malloc(sizeof(text) * includesCount);
        for (u32 i = 0; i < includesCount; i++)
        {
            includeDirectories[i] = argv[i + 2];
        }
    }

    AssetcShaderCompilerInitialize();
    ShaderCompiler *result = ShaderCompiler_New(includeDirectories, includesCount);

    ArenaAllocator arena = ArenaAllocator(GetCAllocator());
    Scope(ArenaAllocator, arena);
    collections::Array<string> allFilePaths = io::GetFilesInDirectoryRecursive(arena.AsAllocator(), argv[1]);

    i32 returnCode;
    for (u32 i = 0; i < allFilePaths.length; i++)
    {
        if (allFilePaths[i].EndsWith(".slang") && !allFilePaths[i].EndsWith(".inc.slang"))
        {
            string outputPath = path::SwapExtension(arena.AsAllocator(), allFilePaths[i], ".func");

            returnCode = ShaderCompiler_Compile(result, allFilePaths[i].buffer, outputPath.buffer);
            if (returnCode != 0)
            {
                text error = ShaderCompiler_GetErrorMessages(result);
                fprintf(stderr, "%s\n", error);
                break;
            }
        }
    }

    if (includeDirectories != NULL)
    {
        free(includeDirectories);
    }

    ShaderCompiler_Deinit(result);
    AssetcShaderCompilerUnload();

    return returnCode;
}