#include "lgfx/lgfx-glfw.h"
#include "lgfx/lgfx.h"
#include "Linxc.h"
#include <stdio.h>
#include <assert.h>
#include "Maths/Vec4.hpp"
#include "Maths/Quaternion.hpp"

struct VertexPositionColorTexture
{
    Maths::Vec4 position;
    Maths::Vec4 color;
    Maths::Vec2 UV;
};
struct VertexLinePosition
{
    Maths::Vec4 position_lineWidth;
    Maths::Vec4 color;
    Maths::Quaternion direction;
    Maths::Vec2 UV;
};
struct VertexPositionNormalTexture
{
    Maths::Vec3 position;
    Maths::Vec3 normal;
    Maths::Vec2 UV;
};
struct VertexPositionNormalTextureBones
{
    Maths::Vec3 position;
    Maths::Vec3 normal;
    Maths::Vec2 UV;
    i32 boneIDs[4];
    Maths::Vec4 weights;
};
struct TerrainBrush2DVertex
{
    Maths::Vec2 position;
    i32 outwardNormals_instanceIndex;
    u32 uv;
    u32 color;
};

i32 main()
{
    //must be a multiple of the largest field size, or a multiple of 
    printf("Testing LGFXCreateVertexDeclaration sizes");

    LGFXVertexElementFormat formats[16];
    //LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, uint32_t elementsCount, bool isPerInstance, bool tightlyPacked)

    formats[0] = LGFXVertexElementFormat_Vector4;
    formats[1] = LGFXVertexElementFormat_Vector4;
    formats[2] = LGFXVertexElementFormat_Vector2;

    LGFXVertexDeclaration decl = LGFXCreateVertexDeclaration(formats, 3, false, false);
    assert(decl.packedSize == sizeof(VertexPositionColorTexture));
    
    
    formats[0] = LGFXVertexElementFormat_Vector4;
    formats[1] = LGFXVertexElementFormat_Vector4;
    formats[2] = LGFXVertexElementFormat_Vector4;
    formats[3] = LGFXVertexElementFormat_Vector2;

    decl = LGFXCreateVertexDeclaration(formats, 4, false, false);
    assert(decl.packedSize == sizeof(VertexLinePosition));


    formats[0] = LGFXVertexElementFormat_Vector3;
    formats[1] = LGFXVertexElementFormat_Vector3;
    formats[2] = LGFXVertexElementFormat_Vector2;

    decl = LGFXCreateVertexDeclaration(formats, 3, false, false);
    assert(decl.elements[0].offset == offsetof(VertexPositionNormalTexture, position));
    assert(decl.elements[1].offset == offsetof(VertexPositionNormalTexture, normal));
    assert(decl.elements[2].offset == offsetof(VertexPositionNormalTexture, UV));
    assert(decl.packedSize == sizeof(VertexPositionNormalTexture));


    formats[0] = LGFXVertexElementFormat_Vector3;
    formats[1] = LGFXVertexElementFormat_Vector3;
    formats[2] = LGFXVertexElementFormat_Vector2;
    formats[3] = LGFXVertexElementFormat_Int4;
    formats[4] = LGFXVertexElementFormat_Vector4;

    decl = LGFXCreateVertexDeclaration(formats, 5, false, false);
    assert(decl.elements[1].offset == offsetof(VertexPositionNormalTextureBones, normal));
    assert(decl.elements[2].offset == offsetof(VertexPositionNormalTextureBones, UV));
    assert(decl.packedSize == sizeof(VertexPositionNormalTextureBones));

    formats[0] = LGFXVertexElementFormat_Vector2;
    formats[1] = LGFXVertexElementFormat_Int;
    formats[2] = LGFXVertexElementFormat_UInt;
    formats[3] = LGFXVertexElementFormat_UInt;
    decl = LGFXCreateVertexDeclaration(formats, 4, false, false);
    assert(decl.elements[0].offset == offsetof(TerrainBrush2DVertex, position));
    assert(decl.elements[1].offset == offsetof(TerrainBrush2DVertex, outwardNormals_instanceIndex));
    assert(decl.elements[2].offset == offsetof(TerrainBrush2DVertex, uv));
    assert(decl.elements[3].offset == offsetof(TerrainBrush2DVertex, color));
    assert(decl.packedSize == sizeof(TerrainBrush2DVertex));

    return 0;
}