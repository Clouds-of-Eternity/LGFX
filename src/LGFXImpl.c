#include "lgfx/lgfx.h"
#include "memory.h"
#include "Logging.h"
#include <math.h>

#include "vulkan/LGFXVulkan.h"

u8 LGFXGetPixelSize(LGFXTextureFormat format)
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

LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, u32 elementsCount, bool isPerInstance, bool tightlyPacked)
{
    LGFXVertexDeclaration result = {0};
    result.elements = Allocate(LGFXVertexAttribute, elementsCount);
    result.elementsCount = elementsCount;

    u32 total = 0;
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
            case LGFXVertexElementFormat_Color:
            case LGFXVertexElementFormat_Uint:
            case LGFXVertexElementFormat_Int:
            {
                LGFXVertexElementFormat prevFormat = LGFXVertexElementFormat_Invalid;
                if (i >= 1)
                {
                    prevFormat = result.elements[i - 1].format;
                }
                if (!tightlyPacked && prevFormat != LGFXVertexElementFormat_Invalid)
                {
                    if (prevFormat != LGFXVertexElementFormat_Float && prevFormat != LGFXVertexElementFormat_Uint && prevFormat != LGFXVertexElementFormat_Int)
                    {
                        if (total % 8 != 0)
                        {
                            total = (u32)ceilf((float)total / 8.0f - 0.01f) * 8;
                        }
                    }
                }
                result.elements[i].offset = total;
                total += 4;
                break;
            }
            case LGFXVertexElementFormat_Vector2:
            {
                if (!tightlyPacked)
                {
                    if (total % 8 != 0)
                    {
                        total = (u32)ceilf((float)total / 8.0f - 0.01f) * 8;
                    }
                }
                result.elements[i].offset = total;
                total += 8;
                break;
            }
            case LGFXVertexElementFormat_Vector3:
            {
                //utterly cursed attribute format
                if (!tightlyPacked)
                {
                    if (total % 12 != 0 && total % 16 != 0)
                    {
                        u32 a = (u32)ceilf((float)total / 12.0f - 0.01f) * 12;
                        u32 b = (u32)ceilf((float)total / 16.0f - 0.01f) * 16;
                        total = a < b ? a : b;
                    }
                }
                result.elements[i].offset = total;
                total += 12;
                break;
            }
            case LGFXVertexElementFormat_Vector4:
            {
                if (!tightlyPacked)
                {
                    if (total % 16 != 0)
                    {
                        total = (u32)ceilf((float)total / 16.0f - 0.01f) * 16;
                    }
                }
                result.elements[i].offset = total;
                total += 16;
                break;
            }
            default:
                break;
        }
    }
    if (!tightlyPacked)
    {
        total = (u32)ceilf((float)total / 16.0f - 0.01f) * 16;
    }

    result.packedSize = total;
    result.isPerInstance = isPerInstance;
    result.isTightlyPacked = tightlyPacked;

    return result;
}

LGFXInstance LGFXCreateInstance(LGFXInstanceCreateInfo *info)
{
    if (info->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateInstance(info);
    }
    LGFX_ERROR("LGFXCreateInstance: Unknown backend\n");
    return NULL;
}
void LGFXDestroyInstance(LGFXInstance instance)
{
    if (instance->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyInstance(instance);
        return;
    }
    LGFX_ERROR("LGFXDestroyInstance: Unknown backend\n");
}

LGFXFence LGFXCreateFence(LGFXDevice device, bool signalled)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFence(device, signalled);
    }
    LGFX_ERROR("LGFXCreateFence: Unknown backend\n");
    return NULL;
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
LGFXFence LGFXRentFence(LGFXDevice device, bool signalled)
{
    return LGFXFencePool_Rent(&device->fencePool, device, signalled);
}
void LGFXReturnRentedFence(LGFXDevice device, LGFXFence fence)
{
    LGFXFencePool_Return(&device->fencePool, fence);
}
void LGFXAwaitFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitFence(fence);
        return;
    }
    LGFX_ERROR("VkLGFXAwaitFence: Unknown backend\n");
}
void LGFXResetFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXResetFence(fence);
        return;
    }
    LGFX_ERROR("VkLGFXResetFence: Unknown backend\n");
}
void LGFXDestroyFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFence(fence);
        return;
    }
    LGFX_ERROR("LGFXDestroyFence: Unknown backend\n");
}

LGFXSemaphore LGFXCreateSemaphore(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSemaphore(device);
    }
    LGFX_ERROR("LGFXCreateSemaphore: Unknown backend\n");
    return NULL;
}
void LGFXDestroySemaphore(LGFXSemaphore semaphore)
{
    if (semaphore->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySemaphore(semaphore);
        return;
    }
    LGFX_ERROR("LGFXDestroySemaphore: Unknown backend\n");
}
LGFXSemaphore LGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetAwaitRenderedSemaphore(swapchain);
    }
    LGFX_ERROR("LGFXSwapchainGetAwaitRenderedSemaphore: Unknown backend\n");
    return NULL; // return something temporary
}
LGFXSemaphore LGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetAwaitPresentedSemaphore(swapchain);
    }
    LGFX_ERROR("LGFXSwapchainGetAwaitPresentedSemaphore: Unknown backend\n");
    return NULL; // return something temporary
}

void LGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitWriteFunction(commandBuffer, funcType, opType);
        return;
    }
    LGFX_ERROR("LGFXAwaitWriteFunction: Unknown backend\n");
}
void LGFXAwaitDraw(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitDraw(commandBuffer);
        return;
    }
    LGFX_ERROR("LGFXAwaitDraw: Unknown backend\n");
}
void LGFXAwaitGraphicsIdle(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitGraphicsIdle(device);
        return;
    }
    LGFX_ERROR("LGFXAwaitGraphicsIdle: Unknown backend\n");
}

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info)
{
    if (instance->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateDevice(instance, info);
    }
    LGFX_ERROR("LGFXCreateDevice: Unknown backend\n");
    return NULL;
}
void LGFXDestroyDevice(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyDevice(device);
        return;
    }
    LGFX_ERROR("LGFXDestroyDevice: Unknown backend\n");
}

LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSwapchain(device, info);
    }
    LGFX_ERROR("LGFXCreateSwapchain: Unknown backend\n");
    return NULL;
}
void LGFXAwaitSwapchainIdle(LGFXSwapchain swapchain)
{
    LGFXAwaitFence(swapchain->fence);
}
u32 LGFXSwapchainGetBackbufferTexturesCount(LGFXSwapchain swapchain)
{
    return swapchain->backbufferTexturesCount;
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
void LGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySwapchain(swapchain, windowIsDestroyed);
        return;
    }
    LGFX_ERROR("LGFXDestroySwapchain: Unknown backend\n");
}

LGFXTexture LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateTexture(device, info);
    }
    LGFX_ERROR("LGFXCreateTexture: Unknown backend\n");
    return NULL;
}
void LGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXTextureTransitionLayout(device, texture, targetLayout, commandBuffer, mipToTransition, mipTransitionDepth);
        return;
    }
    LGFX_ERROR("LGFXTextureTransitionLayout: Unknown backend\n");
}
void LGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXTextureSetData(device, texture, bytes, length);
        return;
    }
    LGFX_ERROR("LGFXTextureSetData: Unknown backend\n");
}
void LGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToTexture(device, commandBuffer, from, to, toMip);
        return;
    }
    LGFX_ERROR("LGFXCopyBufferToTexture: Unknown backend\n");
}
void LGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, LGFXPoint3 extents, LGFXPoint3 offset, u32 toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToTextureWithExtents(device, commandBuffer, from, to, extents, offset, toMip);
        return;
    }
    LGFX_ERROR("LGFXCopyBufferToTextureWithExtents: Unknown backend\n");
}
void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyTextureToBuffer(device, commandBuffer, from, to, toMip);
        return;
    }
    LGFX_ERROR("LGFXCopyTextureToBuffer: Unknown backend\n");
}
void LGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, u32 fromMip, LGFXPoint3 toOffset, u32 toMip, LGFXPoint3 copyAreaSize, bool autoTransition)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyTextureToTexture(device, commandBuffer, from, to, fromOffset, fromMip, toOffset, toMip, copyAreaSize, autoTransition);
        return;
    }
    LGFX_ERROR("LGFXCopyTextureToBuffer: Unknown backend\n");
}
void LGFXDestroyTexture(LGFXTexture texture)
{
    if (texture->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyTexture(texture);
        return;
    }
    LGFX_ERROR("LGFXDestroyTexture: Unknown backend\n");
}

LGFXSamplerState LGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSamplerState(device, info);
    }
    LGFX_ERROR("LGFXCreateSamplerState: Unknown backend\n");
    return NULL;
}
void LGFXDestroySamplerState(LGFXSamplerState state)
{
    if (state->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySamplerState(state);
        return;
    }
    LGFX_ERROR("LGFXDestroySamplerState: Unknown backend\n");
}

LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderTarget(device, info);
    }
    LGFX_ERROR("LGFXCreateRenderTarget: Unknown backend\n");
    return NULL;
}
void LGFXDestroyRenderTarget(LGFXRenderTarget target)
{
    if (target->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyRenderTarget(target);
        return;
    }
    LGFX_ERROR("LGFXDestroyRenderTarget: Unknown backend\n");
}

LGFXBuffer LGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateBuffer(device, info);
    }
    LGFX_ERROR("LGFXCreateBuffer: Unknown backend\n");
    return NULL;
}
void LGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToBuffer(device, commandBuffer, from, to);
        return;
    }
    LGFX_ERROR("LGFXCopyBufferToBuffer: Unknown backend\n");
}
void LGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, u8 *data, usize dataLength)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetBufferDataOptimizedData(buffer, commandBufferToUse, data, dataLength);
        return;
    }
    LGFX_ERROR("LGFXSetBufferDataOptimizedData: Unknown backend\n");
}
void LGFXSetBufferDataFast(LGFXBuffer buffer, u8 *data, usize dataLength)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetBufferDataFast(buffer, data, dataLength);
        return;
    }
    LGFX_ERROR("LGFXSetBufferDataFast: Unknown backend\n");
}
void LGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, u32 value)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXFillBuffer(cmdBuffer, buffer, value);
        return;
    }
    LGFX_ERROR("LGFXFillBuffer: Unknown backend\n");
}
void LGFXDestroyBuffer(LGFXBuffer buffer)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyBuffer(buffer);
        return;
    }
    LGFX_ERROR("LGFXDestroyBuffer: Unknown backend\n");
}
void *LGFXGetBufferData(LGFXBuffer buffer, usize *bytesLength)
{
    if (bytesLength != NULL)
    {
        *bytesLength = buffer->size;
    }
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXGetBufferData(buffer);
    }
    LGFX_ERROR("LGFXGetBufferData: Unknown backend\n");
    return NULL;
}
void *LGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(usize))
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXReadBufferFromGPU(buffer, allocateFunction);
    }
    LGFX_ERROR("LGFXReadBufferFromGPU: Unknown backend\n");
    return NULL;
}

LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderProgram(device, info);
    }
    LGFX_ERROR("LGFXCreateRenderProgram: Unknown backend\n");
    return NULL;
}
void LGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXBeginRenderProgramSwapchain(program, commandBuffer, outputSwapchain, clearColor, autoTransitionTargetTextures);
        return;
    }
    LGFX_ERROR("LGFXBeginRenderProgram: Unknown backend\n");
}
void LGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXBeginRenderProgram(program, commandBuffer, outputTarget, clearColor, autoTransitionTargetTextures);
        return;
    }
    LGFX_ERROR("LGFXBeginRenderProgram: Unknown backend\n");
}
void LGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXRenderProgramNextPass(commandBuffer);
        return;
    }
    LGFX_ERROR("LGFXRenderProgramNextPass: Unknown backend\n");
}
void LGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXEndRenderProgram(program, commandBuffer);
        return;
    }
    LGFX_ERROR("LGFXEndRenderProgram: Unknown backend\n");
}
void LGFXDestroyRenderProgram(LGFXRenderProgram program)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyRenderProgram(program);
        return;
    }
    LGFX_ERROR("LGFXDestroyRenderProgram: Unknown backend\n");
}

LGFXFunctionVariableBatchTemplate LGFXCreateFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableBatchTemplate(device, info);
    }
    LGFX_ERROR("LGFXCreateFunctionVariableBatchTemplate: Unknown backend\n");
    return NULL;
}
LGFXFunctionVariableBatch LGFXCreateFunctionVariableBatchFromTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate fromTemplate)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableBatchFromTemplate(device, fromTemplate);
    }
    LGFX_ERROR("LGFXCreateFunctionVariableBatch: Unknown backend\n");
    return NULL;
}
void LGFXDestroyFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate toDestroy)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFunctionVariableBatchTemplate(device, toDestroy);
        return;
    }
    LGFX_ERROR("LGFXDestroyFunctionVariableBatchTemplate: Unknown backend\n");
}

LGFXFunction LGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunction(device, info);
    }
    LGFX_ERROR("LGFXCreateFunction: Unknown backend\n");
    return NULL;
}
void LGFXDestroyFunction(LGFXFunction func)
{
    if (func->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFunction(func);
        return;
    }
    LGFX_ERROR("LGFXDestroyFunction: Unknown backend\n");
}
LGFXFunctionVariableBatch LGFXCreateFunctionVariableBatch(LGFXFunction function)
{
    if (function->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableBatch(function);
    }
    LGFX_ERROR("LGFXFunctionGetVariableBatch: Unknown backend\n");
    return NULL;
}
LGFXFunctionVariable LGFXCreateFunctionVariable(LGFXDevice device, LGFXShaderResource *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariable(device, info);
    }
    LGFX_ERROR("LGFXCreateFunctionVariable: Unknown backend\n");
    LGFXFunctionVariable empty = {0};
    return empty;
}
LGFXFunctionVariable LGFXCreateFunctionVariableSlot(LGFXFunction function, u32 forVariableOfIndex)
{
    if (function->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableSlot(function, forVariableOfIndex);
    }
    LGFX_ERROR("LGFXFunctionCreateVariableSlot: Unknown backend\n");
    LGFXFunctionVariable empty = {0};
    return empty;
}
void LGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *functionVariables, u32 variablesCount)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXFunctionSendVariablesToGPU(device, batch, functionVariables, variablesCount);
        return;
    }
    LGFX_ERROR("LGFXFunctionSendVariablesToGPU: Unknown backend\n");
}
void LGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction, u32 setIndex)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseFunctionVariables(commandBuffer, batch, forFunction, setIndex);
        return;
    }
    LGFX_ERROR("LGFXUseFunctionVariables: Unknown backend\n");
}
void LGFXDestroyFunctionVariable(LGFXFunctionVariable variable)
{
    if (variable.device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFunctionVariable(variable);
        return;
    }
    LGFX_ERROR("LGFXDestroyFunctionVariable: Unknown backend\n");
}

LGFXShaderState LGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateShaderState(device, info);
    }
    LGFX_ERROR("LGFXCreateShaderState: Unknown backend\n");
    return NULL;
}
void LGFXDestroyShaderState(LGFXShaderState shaderState)
{
    if (shaderState->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyShaderState(shaderState);
        return;
    }
    LGFX_ERROR("LGFXDestroyShaderState: Unknown backend\n");
}
void LGFXUseShaderState(LGFXCommandBuffer buffer, LGFXShaderState shaderState)
{
    if (shaderState->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseShaderState(buffer, shaderState);
        return;
    }
    LGFX_ERROR("LGFXUseShaderState: Unknown backend\n");
}


void LGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetViewport(commandBuffer, area);
        return;
    }
    LGFX_ERROR("LGFXSetViewport: Unknown backend\n");
}
void LGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetClipArea(commandBuffer, area);
        return;
    }
    LGFX_ERROR("LGFXSetClipArea: Unknown backend\n");
}

LGFXCommandBuffer LGFXCreateCommandBuffer(LGFXDevice device, bool forCompute)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateCommandBuffer(device, forCompute);
    }
    LGFX_ERROR("LGFXCommandBuffer: Unknown backend\n");
    return NULL;
}
void LGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferBegin(buffer, resetAfterSubmission);
        return;
    }
    LGFX_ERROR("LGFXCommandBufferBegin: Unknown backend\n");
}
void LGFXCommandBufferEnd(LGFXCommandBuffer buffer)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferEnd(buffer);
        return;
    }
    LGFX_ERROR("LGFXCommandBufferEnd: Unknown backend\n");
}
void LGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferExecute(buffer, fence, awaitSemaphore, signalSemaphore);
        return;
    }
    LGFX_ERROR("LGFXCommandBufferEnd: Unknown backend\n");
}
void LGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferEndSwapchain(buffer, swapchain);
        return;
    }
    LGFX_ERROR("LGFXCommandBufferEndSwapchain: Unknown backend\n");
}
void LGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyCommandBuffer(commandBuffer);
        return;
    }
    LGFX_ERROR("LGFXDestroyCommandBuffer: Unknown backend\n");
}
void LGFXCommandBufferReset(LGFXCommandBuffer buffer)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferReset(buffer);
        return;
    }
    LGFX_ERROR("LGFXCommandBufferReset: Unknown backend\n");
}

void LGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, usize offset)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseIndexBuffer(commands, indexBuffer, offset);
        return;
    }
    LGFX_ERROR("LGFXUseIndexBuffer: Unknown backend\n");
}
void LGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, u32 vertexBuffersCount)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseVertexBuffer(commands, vertexBuffers, vertexBuffersCount);
        return;
    }
    LGFX_ERROR("LGFXUseVertexBuffer: Unknown backend\n");
}
void LGFXDrawIndexed(LGFXCommandBuffer commands, u32 indexCount, u32 instances, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDrawIndexed(commands, indexCount, instances, firstIndex, vertexOffset, firstInstance);
        return;
    }
    LGFX_ERROR("LGFXDrawIndexed: Unknown backend\n");
}
void LGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, usize bufferOffset, usize drawCount, usize drawParamsStride)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDrawIndexedIndirect(commands, drawParamsBuffer, bufferOffset, drawCount, drawParamsStride);
        return;
    }
    LGFX_ERROR("LGFXDrawIndexedIndirect: Unknown backend\n");
}

void LGFXDispatchCompute(LGFXCommandBuffer commands, u32 groupsX, u32 groupsY, u32 groupsZ)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDispatchCompute(commands, groupsX, groupsY, groupsZ);
        return;
    }
    LGFX_ERROR("LGFXDispatchCompute: Unknown backend\n");
}
void LGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, usize offset)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDispatchComputeIndirect(commands, dispatchParamsBuffer, offset);
        return;
    }
    LGFX_ERROR("LGFXDispatchComputeIndirect: Unknown backend\n");
}

bool LGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXNewFrame(device, swapchain, frameWidth, frameHeight);
    }
    LGFX_ERROR("LGFXNewFrame: Unknown backend\n");
    return NULL;
}
void LGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSubmitFrame(device, swapchain);
        return;
    }
    LGFX_ERROR("LGFXSubmitFrame: Unknown backend\n");
}