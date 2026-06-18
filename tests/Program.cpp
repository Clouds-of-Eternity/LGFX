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

i32 main()
{
    printf("Testing LGFXCreateVertexDeclaration sizes");

    LGFXVertexElementFormat formats[16];
    //LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, uint32_t elementsCount, bool isPerInstance, bool tightlyPacked)

    formats[0] = LGFXVertexElementFormat_Vector4;
    formats[1] = LGFXVertexElementFormat_Vector4;
    formats[2] = LGFXVertexElementFormat_Vector2;

    LGFXVertexDeclaration decl = LGFXCreateVertexDeclaration(formats, 3, false, false);
    assert(decl.packedSize == sizeof(VertexPositionColorTexture));

    return 0;
}