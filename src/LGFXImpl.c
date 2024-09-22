#include "lgfx/lgfx.h"
#include "memory.h"
#include "Logging.h"
#include <math.h>

#include "vulkan/LGFXVulkan.h"

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

LGFXInstance LGFXCreateInstance(LGFXInstanceCreateInfo *info)
{
    if (info->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateInstance(info);
    }
    LGFX_ERROR("LGFXCreateInstance: Unknown backend\n");
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

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info)
{
    if (instance->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateDevice(instance, info);
    }
    LGFX_ERROR("LGFXCreateDevice: Unknown backend\n");
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
}
void LGFXDestroySwapchain(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySwapchain(swapchain);
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
void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyTextureToBuffer(device, commandBuffer, from, to, toMip);
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

LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderTarget(device, info);
    }
    LGFX_ERROR("LGFXCreateRenderTarget: Unknown backend\n");
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

LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderProgram(device, info);
    }
    LGFX_ERROR("LGFXCreateRenderProgram: Unknown backend\n");
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

LGFXFunction LGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunction(device, info);
    }
    LGFX_ERROR("LGFXCreateFunction: Unknown backend\n");
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

LGFXShaderState LGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateShaderState(device, info);
    }
    LGFX_ERROR("LGFXCreateShaderState: Unknown backend\n");
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