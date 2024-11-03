#pragma once
#include "Linxc.h"
#include "array.hpp"
#include "Maths/Vec4.hpp"
#include "lgfx/lgfx.h"
#include "lgfx-astral/Shader.hpp"

struct Color
{
    u8 R;
    u8 G;
    u8 B;
    u8 A;
};
struct Vertex
{
    Maths::Vec4 position;
    Maths::Vec4 normal;
};
struct ShaderMeshMetadata
{
    u32 iterationWidth;
    u32 iterationHeight;
    u32 iterationDepth;
    u32 indicesCount;
    Maths::Vec4 minExtents;
    Maths::Vec4 maxExtents;
};
struct Model
{
    collections::Array<Vertex> vertices;
    collections::Array<u32> indices;
    Maths::Vec3 minBounds;
    Maths::Vec3 maxBounds;

    LGFXBuffer vertexBuffer;
    LGFXBuffer indexBuffer;
    LGFXTexture meshSDFTexture;

    inline void deinit()
    {
        vertices.deinit();
        indices.deinit();
        if (vertexBuffer != NULL)
        {
            LGFXDestroyBuffer(vertexBuffer);
        }
        if (indexBuffer != NULL)
        {
            LGFXDestroyBuffer(indexBuffer);
        }
        // if (pcBuffer != NULL)
        // {
        //     LGFXDestroyBuffer(pcBuffer);
        // }
        if (meshSDFTexture != NULL)
        {
            LGFXDestroyTexture(meshSDFTexture);
        }
    }
};

Model LoadModel(text fileName, AstralCanvas::Shader *loadShader, LGFXShaderState loadShaderState);