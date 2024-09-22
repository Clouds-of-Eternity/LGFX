#include "lgfx/lgfx.h"
#include "memory.h"
#include "Logging.h"
#include <math.h>

LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, u32 elementsCount, bool isPerInstance, bool tightlyPacked)
{
    LGFXVertexDeclaration result = {0};
    result.elements = Allocate(LGFXVertexAttribute, elementsCount);
    result.elementsCount = elementsCount;

    u32 total = 0;
    LGFXVertexElementFormat prevFormat = LGFXVertexElementFormat_Invalid;
    for (u32 i = 0; i < elementsCount; i++)
    {
        result.elements[i].format = elementFormats[i];
        switch(elementFormats[i])
        {
            case LGFXVertexElementFormat_Float:
            {
                result.elements[i].offset = total;
                total += 4;
                break;
            }
            case LGFXVertexElementFormat_Uint:
            case LGFXVertexElementFormat_Int:
            {
                if (prevFormat == LGFXVertexElementFormat_Float || prevFormat == LGFXVertexElementFormat_Uint || prevFormat == LGFXVertexElementFormat_Int)
                {
                    total += 4;
                }
                result.elements[i].offset = total;
                total += 4;
                break;
            }
            case LGFXVertexElementFormat_Vector2:
            {
                if (total % 8 != 0)
                {
                    total = (u32)ceilf((float)total / 8.0f - 0.01f) * 8;
                }
                result.elements[i].offset = total;
                total += 8;
                break;
            }
            case LGFXVertexElementFormat_Vector3:
            {
                //utterly cursed attribute format
                if (total % 12 != 0 && total % 16 != 0)
                {
                    total = min((u32)ceilf((float)total / 12.0f - 0.01f) * 12, (u32)ceilf((float)total / 16.0f - 0.01f) * 16);
                }
                result.elements[i].offset = total;
                total += 12;
                break;
            }
            case LGFXVertexElementFormat_Vector4:
            {
                if (total % 16 != 0)
                {
                    total = (u32)ceilf((float)total / 16.0f - 0.01f) * 16;
                }
                result.elements[i].offset = total;
                total += 16;
                break;
            }
            default:
                break;
        }
        prevFormat = elementFormats[i];
    }

    result.packedSize = total;
    result.isPerInstance = isPerInstance;

    return result;
}