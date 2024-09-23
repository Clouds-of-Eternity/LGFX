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
void VkLGFXAwaitFence(LGFXFence fence);
void VkLGFXResetFence(LGFXFence fence);
void VkLGFXDestroyFence(LGFXFence fence);

LGFXSemaphore VkLGFXCreateSemaphore(LGFXDevice device);
void VkLGFXDestroySemaphore(LGFXSemaphore semaphore);

LGFXDevice VkLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void VkLGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain VkLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void VkLGFXDestroySwapchain(LGFXSwapchain swapchain);

bool VkLGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight);
void VkLGFXSubmitFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight);

LGFXTexture VkLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void VkLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth);
void VkLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length);
void VkLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip);
void VkLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip);
void VkLGFXDestroyTexture(LGFXTexture texture);

LGFXSamplerState VkLGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info);
void VkLGFXDestroySamplerState(LGFXSamplerState state);

LGFXRenderTarget VkLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void VkLGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer VkLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void VkLGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to);
void VkLGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, u8 *data, usize dataLength);
void VkLGFXSetBufferDataFast(LGFXBuffer buffer, u8 *data, usize dataLength);
void VkLGFXDestroyBuffer(LGFXBuffer buffer);

LGFXRenderProgram VkLGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void VkLGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures);
void VkLGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures);
void VkLGFXEndRenderProgram(LGFXCommandBuffer commandBuffer);
void VkLGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXFunction VkLGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info);
void VkLGFXDestroyFunction(LGFXFunction func);
LGFXFunctionVariable VkLGFXFunctionGetVariableSlot(LGFXFunction function, u32 forVariableOfIndex);
void VkLGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariable *shaderVariables, u32 shaderVariableCount);
void VkLGFXDestroyFunctionVariable(LGFXFunctionVariable variable);

LGFXShaderState VkLGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info);
void VkLGFXDestroyShaderState(LGFXShaderState shaderState);
void VkLGFXUseShaderState(LGFXCommandBuffer buffer, LGFXShaderState shaderState);

LGFXCommandBuffer VkLGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
void VkLGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
void VkLGFXCommandBufferEnd(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
void VkLGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain);
void VkLGFXCommandBufferReset(LGFXCommandBuffer buffer);
void VkLGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

void VkLGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area);
void VkLGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area);

void VkLGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, usize offset);
void VkLGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, u32 vertexBuffersCount);
void VkLGFXDrawIndexed(LGFXCommandBuffer commands, u32 indexCount, u32 instances, u32 firstIndex, u32 vertexOffset, u32 firstInstance);

#endif