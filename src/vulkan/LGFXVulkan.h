#ifndef LGFXVULKAN_HPP
#define LGFXVULKAN_HPP

#include "LGFXImpl.h"

#define LGFX_VULKAN_API_VERSION VK_API_VERSION_1_3

typedef struct VkLGFXQueueProperties
{
    void *allQueueInfos;
    uint32_t allQueueInfosCount;
    
    int32_t generalPurposeQueueIndex;
    int32_t dedicatedGraphicsQueueIndex;
    int32_t dedicatedComputeQueueIndex;
    int32_t dedicatedTransferQueueIndex;
} VkLGFXQueueProperties;

LGFXInstance VkLGFXCreateInstance(LGFXInstanceCreateInfo *info);
void VkLGFXDestroyInstance(LGFXInstance instance);

LGFXFence VkLGFXCreateFence(LGFXDevice device, bool signalled);
void VkLGFXAwaitFence(LGFXFence fence);
void VkLGFXResetFence(LGFXFence fence);
void VkLGFXDestroyFence(LGFXFence fence);

LGFXSemaphore VkLGFXCreateSemaphore(LGFXDevice device);
void VkLGFXDestroySemaphore(LGFXSemaphore semaphore);

void VkLGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType);
void VkLGFXAwaitDraw(LGFXCommandBuffer commandBuffer);
void VkLGFXAwaitGraphicsIdle(LGFXDevice device);

LGFXDevice VkLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void VkLGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain VkLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void VkLGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed);
LGFXSemaphore VkLGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain);
LGFXSemaphore VkLGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain);

bool VkLGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, uint32_t frameWidth, uint32_t frameHeight);
void VkLGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain);

LGFXTexture VkLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void VkLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, uint32_t mipToTransition, uint32_t mipTransitionDepth);
void VkLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, uint8_t* bytes, size_t length);
void VkLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, uint32_t toMip);
void VkLGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, LGFXPoint3 extents, LGFXPoint3 offset, uint32_t toMip);
void VkLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, uint32_t toMip);
void VkLGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, uint32_t fromMip, LGFXPoint3 toOffset, uint32_t toMip, LGFXPoint3 copyAreaSize, bool autoTransition);
void VkLGFXDestroyTexture(LGFXTexture texture);
// void VkLGFXTextureBlit(LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to);

LGFXSamplerState VkLGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info);
void VkLGFXDestroySamplerState(LGFXSamplerState state);

LGFXRenderTarget VkLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void VkLGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer VkLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void VkLGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to);
void VkLGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, uint8_t *data, size_t dataLength);
void VkLGFXSetBufferDataFast(LGFXBuffer buffer, uint8_t *data, size_t dataLength);
void VkLGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, uint32_t value);
void VkLGFXDestroyBuffer(LGFXBuffer buffer);
void *VkLGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(size_t));
void *VkLGFXGetBufferData(LGFXBuffer buffer);

LGFXRenderProgram VkLGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void VkLGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures);
void VkLGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures);
void VkLGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer);
void VkLGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer);
void VkLGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXFunctionVariableBatchTemplate VkLGFXCreateFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplateCreateInfo *info);
LGFXFunctionVariableBatch VkLGFXCreateFunctionVariableBatchFromTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate fromTemplate);
void VkLGFXDestroyFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate toDestroy);

LGFXFunction VkLGFXCreateFunction(LGFXDevice device, const LGFXFunctionCreateInfo *info);
void VkLGFXDestroyFunction(LGFXFunction func);
LGFXFunctionVariableBatch VkLGFXCreateFunctionVariableBatch(LGFXFunction function);
LGFXFunctionVariable VkLGFXCreateFunctionVariableSlot(LGFXFunction function, uint32_t forVariableOfIndex);
LGFXFunctionVariable VkLGFXCreateFunctionVariable(LGFXDevice device, LGFXShaderResource *info);
void VkLGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *shaderVariables, uint32_t shaderVariableCount);
void VkLGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction, uint32_t setIndex);
void VkLGFXDestroyFunctionVariable(LGFXFunctionVariable variable);

LGFXShaderPipeline VkLGFXCreateShaderPipeline(LGFXDevice device, LGFXShaderPipelineCreateInfo *info);
void VkLGFXDestroyShaderPipeline(LGFXShaderPipeline shaderPipeline);
void VkLGFXUseShaderPipeline(LGFXCommandBuffer buffer, LGFXShaderPipeline shaderPipeline);

LGFXCommandBuffer VkLGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
void VkLGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
void VkLGFXCommandBufferEnd(LGFXCommandBuffer buffer);
void VkLGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
void VkLGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain);
void VkLGFXCommandBufferReset(LGFXCommandBuffer buffer);
void VkLGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

void VkLGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area);
void VkLGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area);

void VkLGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, size_t offset);
void VkLGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, uint32_t vertexBuffersCount);
void VkLGFXDrawIndexed(LGFXCommandBuffer commands, uint32_t indexCount, uint32_t instances, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
void VkLGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, size_t bufferOffset, size_t drawCount, size_t drawParamsStride);

void VkLGFXDispatchCompute(LGFXCommandBuffer commands, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ);
void VkLGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, size_t offset);

#endif