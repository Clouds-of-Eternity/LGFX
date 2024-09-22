#ifndef LGFXVULKAN_HPP
#define LGFXVULKAN_HPP

#include "LGFXImpl.h"

#define LGFX_VULKAN_API_VERSION VK_API_VERSION_1_3

typedef struct VkLGFXQueueProperties
{
    void *allQueueInfos;
    u32 allQueueInfosCount;
    
    i32 generalPurposeQueueIndex;
    i32 dedicatedGraphicsQueueIndex;
    i32 dedicatedComputeQueueIndex;
    i32 dedicatedTransferQueueIndex;
} VkLGFXQueueProperties;

LGFXInstance VkLGFXCreateInstance(LGFXInstanceCreateInfo *info);
void VkLGFXDestroyInstance(LGFXInstance instance);

LGFXFence VkLGFXCreateFence(LGFXDevice device, bool signalled);
void VkLGFXDestroyFence(LGFXFence fence);

LGFXDevice VkLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void VkLGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain VkLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void VkLGFXDestroySwapcahin(LGFXSwapchain swapchain);

LGFXTexture VkLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void VkLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth);
void VkLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length);
void VkLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip);
void VkLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip);
void VkLGFXDestroyTexture(LGFXTexture texture);

LGFXRenderTarget VkLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void VkLGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer VkLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void VkLGFXDestroyBuffer(LGFXBuffer buffer);

LGFXRenderProgram VkLGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void VkLGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXMemoryBlockImpl VkLGFXAllocMemoryForTexture(LGFXDevice device, LGFXTexture texture, LGFXMemoryUsage memoryUsage);
LGFXMemoryBlockImpl VkLGFXAllocMemoryForBuffer(LGFXDevice device, LGFXBuffer buffer, LGFXMemoryUsage memoryUsage);

LGFXFunction VkLGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info);
void VkLGFXDestroyFunction(LGFXFunction func);

LGFXShader VkLGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info);

#endif