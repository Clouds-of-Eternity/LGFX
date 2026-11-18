#include "LGFXImpl.h"
#include "LGFXMemory.h"
#include "LGFXLogging.h"

#include <math.h>

void LGFXUseVertexBuffer(LGFXCommandBuffer commands, const LGFXBuffer *vertexBuffers, uint32_t vertexBuffersCount)
{
    LGFXUseVertexBuffer2(commands, vertexBuffers, NULL, vertexBuffersCount);
}
uint32_t LGFXSwapchainGetBackbufferTexturesCount(LGFXSwapchain swapchain)
{
    return swapchain->backbufferTexturesCount;
}
void LGFXAwaitSwapchainIdle(LGFXSwapchain swapchain)
{
    LGFXAwaitFence(swapchain->frameDatas[swapchain->currentFrameIndex].fence);
}
void LGFXSwapchainInvalidate(LGFXSwapchain swapchain)
{
    swapchain->invalidated = true;
}
void LGFXSwapchainSetPresentationMode(LGFXSwapchain swapchain, LGFXSwapchainPresentationMode mode)
{
    swapchain->invalidated = true;
    swapchain->presentMode = mode;
}
LGFXFence LGFXFencePool_Rent(LGFXFencePool *pool, LGFXDevice device, bool initiallySignalled)
{
    if (pool->numFences == 0)
    {
        LGFXFence result = LGFXCreateFence(device, initiallySignalled);
        return result;
    }
    else
    {
        pool->numFences--;
        LGFXFence result = pool->fences[pool->numFences];
        pool->fences[pool->numFences] = NULL;
        return result;
    }
}
void LGFXFencePool_Return(LGFXFencePool *pool, LGFXFence fence)
{
    if (pool->numFences < LGFX_FENCE_POOL_SIZE)
    {
        LGFXResetFence(fence);
        pool->fences[pool->numFences] = fence;
        pool->numFences++;
    }
    else
    {
        LGFXDestroyFence(fence);
    }
}

uint8_t LGFXGetPixelSize(LGFXTextureFormat format)
{
    switch (format)
    {
        //compression formats have a per-bit pixel size
        
        case LGFXTextureFormat_Undefined:
            return 0;
        case LGFXTextureFormat_R8Unorm:
        case LGFXTextureFormat_R8Snorm:
        case LGFXTextureFormat_R8Uint:
        case LGFXTextureFormat_R8Sint:
            return 1;
        case LGFXTextureFormat_R16Uint:
        case LGFXTextureFormat_R16Sint:
        case LGFXTextureFormat_R16Float:
        case LGFXTextureFormat_RG8Unorm:
        case LGFXTextureFormat_RG8Snorm:
        case LGFXTextureFormat_RG8Uint:
        case LGFXTextureFormat_RG8Sint:
            return 2;
        case LGFXTextureFormat_R32Float:
        case LGFXTextureFormat_R32Uint:
        case LGFXTextureFormat_R32Sint:
        case LGFXTextureFormat_RG16Uint:
        case LGFXTextureFormat_RG16Sint:
        case LGFXTextureFormat_RG16Float:
        case LGFXTextureFormat_RGBA8Unorm:
        case LGFXTextureFormat_RGBA8UnormSrgb:
        case LGFXTextureFormat_RGBA8Snorm:
        case LGFXTextureFormat_RGBA8Uint:
        case LGFXTextureFormat_RGBA8Sint:
        case LGFXTextureFormat_BGRA8Unorm:
        case LGFXTextureFormat_BGRA8UnormSrgb:
        case LGFXTextureFormat_RGB10A2Uint:
        case LGFXTextureFormat_RGB10A2Unorm:
        case LGFXTextureFormat_RG11B10Ufloat:
        case LGFXTextureFormat_RGB9E5Ufloat:
            return 4;
        case LGFXTextureFormat_RG32Float:
        case LGFXTextureFormat_RG32Uint:
        case LGFXTextureFormat_RG32Sint:
        case LGFXTextureFormat_RGBA16Uint:
        case LGFXTextureFormat_RGBA16Sint:
        case LGFXTextureFormat_RGBA16Float:
            return 8;
        case LGFXTextureFormat_RGBA32Float:
        case LGFXTextureFormat_RGBA32Uint:
        case LGFXTextureFormat_RGBA32Sint:
            return 16;
        case LGFXTextureFormat_Stencil8:
            return 1;
        case LGFXTextureFormat_Depth16Unorm:
            return 2;
        case LGFXTextureFormat_Depth24Plus:
            return 3;
        case LGFXTextureFormat_Depth24PlusStencil8:
            return 4;
        case LGFXTextureFormat_Depth32Float:
            return 4;
        case LGFXTextureFormat_Depth32FloatStencil8:
            return 5;
        default:
            return 0;
    }
}
LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, uint32_t elementsCount, bool isPerInstance, bool tightlyPacked)
{
    LGFXVertexDeclaration result = {0};
    result.elements = Allocate(LGFXVertexAttribute, elementsCount);
    result.elementsCount = elementsCount;

    uint32_t currOffset = 0;
    uint32_t prevFieldTypeSize = 0;
    uint32_t maxFieldTypeSize = 0;
    bool lastElementIsVector = false;
    for (uint32_t i = 0; i < elementsCount; i++)
    {
        result.elements[i].format = elementFormats[i];
        uint32_t fieldTypeSize = 0;
        lastElementIsVector = false;

        switch (elementFormats[i])
        {
            default:
                LGFX_WARN("Created vertex attribute at index %u is of an invalid format", i);
                fieldTypeSize = 0;
                break;
            case LGFXVertexElementFormat_Float:
                fieldTypeSize = 4;
                break;
            case LGFXVertexElementFormat_Vector2:
                fieldTypeSize = 8;
                lastElementIsVector = true;
                break;
            case LGFXVertexElementFormat_Vector3:
                fieldTypeSize = 12;
                lastElementIsVector = true;
                break;
            case LGFXVertexElementFormat_Vector4:
                fieldTypeSize = 16;
                lastElementIsVector = true;
                break;
            case LGFXVertexElementFormat_Color:
                fieldTypeSize = 4;
                break;
            case LGFXVertexElementFormat_Int:
                fieldTypeSize = 4;
                break;
            case LGFXVertexElementFormat_Int2:
                fieldTypeSize = 8;
                break;
            case LGFXVertexElementFormat_Int3:
                fieldTypeSize = 12;
                break;
            case LGFXVertexElementFormat_Int4:
                fieldTypeSize = 16;
                break;
            case LGFXVertexElementFormat_UInt:
                fieldTypeSize = 4;
                break;
            case LGFXVertexElementFormat_UInt2:
                fieldTypeSize = 8;
                break;
            case LGFXVertexElementFormat_UInt3:
                fieldTypeSize = 12;
                break;
            case LGFXVertexElementFormat_UInt4:
                fieldTypeSize = 16;
                break;
        }
        if (fieldTypeSize > maxFieldTypeSize)
        {
            maxFieldTypeSize = fieldTypeSize;
        }

        if (currOffset % fieldTypeSize != 0)
        {
            currOffset = ((currOffset / fieldTypeSize) + 1) * fieldTypeSize;
        }
        result.elements[i].offset = currOffset;
        currOffset += fieldTypeSize;
        prevFieldTypeSize = fieldTypeSize;
    }

    if (!tightlyPacked && lastElementIsVector && currOffset % 16 != 0)
    {
        currOffset = (uint32_t)ceilf((float)currOffset / (float)maxFieldTypeSize - 0.01f) * maxFieldTypeSize;
    }
    result.packedSize = currOffset;
    result.isPerInstance = isPerInstance;
    result.isTightlyPacked = tightlyPacked;

    return result;
}
LGFXFence LGFXRentFence(LGFXDevice device, bool signalled)
{
    return LGFXFencePool_Rent(&device->fencePool, device, signalled);
}
void LGFXReturnRentedFence(LGFXDevice device, LGFXFence fence)
{
    LGFXFencePool_Return(&device->fencePool, fence);
}