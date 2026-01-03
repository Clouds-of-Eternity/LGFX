#pragma once
#include "Maths/Vec2.hpp"
#include "Maths/Vec3.hpp"
#include "Maths/Vec4.hpp"
#include "lgfx/lgfx.h"

namespace LGFX
{
    struct VertexPositionColor
    {
        Maths::Vec4 position;
        Maths::Vec4 color;

        inline VertexPositionColor(Maths::Vec3 position, Maths::Vec4 color)
        {
            this->position = Maths::Vec4(position, 0.0f);
            this->color = color;
        }
    };

    inline LGFXVertexDeclaration GetVertexPositionColorDecl()
    {
        LGFXVertexElementFormat formats[2] = {
            LGFXVertexElementFormat_Vector4,
            LGFXVertexElementFormat_Vector4
        };
        return LGFXCreateVertexDeclaration(formats, 2, false, true);
    }
}