#pragma once
#include "LGFXImpl.h"

LGFXInstance wLGFXCreateInstance(LGFXInstanceCreateInfo *info);
void wLGFXDestroyInstance(LGFXInstance instance);

LGFXFence wLGFXCreateFence(LGFXDevice device, bool signalled);
void wLGFXAwaitFence(LGFXFence fence);
void wLGFXResetFence(LGFXFence fence);
void wLGFXDestroyFence(LGFXFence fence);

LGFXSemaphore wLGFXCreateSemaphore(LGFXDevice device);
void wLGFXDestroySemaphore(LGFXSemaphore semaphore);

void wLGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType);
void wLGFXAwaitDraw(LGFXCommandBuffer commandBuffer);
void wLGFXAwaitGraphicsIdle(LGFXDevice device);

LGFXDevice wLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void wLGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain wLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void wLGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed);
LGFXSemaphore wLGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain);
LGFXSemaphore wLGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain);
uint32_t wLGFXSwapchainGetCurrentFrameIndex(LGFXSwapchain swapchain);
uint32_t wLGFXSwapchainGetCurrentImageIndex(LGFXSwapchain swapchain);

bool wLGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, uint32_t frameWidth, uint32_t frameHeight);
void wLGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain);

LGFXTexture wLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void wLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, uint32_t mipToTransition, uint32_t mipTransitionDepth);
void wLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, uint8_t* bytes, size_t length);
void wLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, uint32_t toMip);
void wLGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, LGFXPoint3 extents, LGFXPoint3 offset, uint32_t toMip);
void wLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, uint32_t toMip);
void wLGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, uint32_t fromMip, LGFXPoint3 toOffset, uint32_t toMip, LGFXPoint3 copyAreaSize, bool autoTransition);
void wLGFXDestroyTexture(LGFXTexture texture);
// void wLGFXTextureBlit(LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to);

LGFXSamplerState wLGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info);
void wLGFXDestroySamplerState(LGFXSamplerState state);

LGFXRenderTarget wLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void wLGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer wLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void wLGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to, size_t fromBufferOffset, size_t setIntoBufferOffset);
void wLGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength);
void wLGFXSetBufferDataFast(LGFXBuffer buffer, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength);
void wLGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, uint32_t value);
void wLGFXDestroyBuffer(LGFXBuffer buffer);
void *wLGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(size_t));
void *wLGFXGetBufferData(LGFXBuffer buffer);
bool wLGFXBufferResize(LGFXBuffer buffer, size_t newSize);

LGFXRenderProgram wLGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void wLGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures);
void wLGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures);
void wLGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer);
void wLGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer);
void wLGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXFunctionVariableBatchTemplate wLGFXCreateFunctionVariableBatchTemplate(LGFXDevice device, const LGFXFunctionVariableBatchTemplateCreateInfo *info);
LGFXFunctionVariableBatch wLGFXCreateFunctionVariableBatchFromTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate fromTemplate);
void wLGFXDestroyFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate batchTemplate);

LGFXFunction wLGFXCreateFunction(LGFXDevice device, const LGFXFunctionCreateInfo *info);
void wLGFXDestroyFunction(LGFXFunction func);
LGFXFunctionVariable wLGFXCreateFunctionVariableSlot(LGFXDevice device, LGFXFunctionVariableBatchTemplate batchTemplate, uint32_t forVariableOfIndex);
LGFXFunctionVariable wLGFXCreateFunctionVariable(LGFXDevice device, LGFXFunctionVariableMetadata *info);
void wLGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *shaderVariables, uint32_t shaderVariableCount);
void wLGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction, uint32_t setIndex);
void wLGFXDestroyFunctionVariable(LGFXFunctionVariable variable);

LGFXShaderPipeline wLGFXCreateShaderPipeline(LGFXDevice device, LGFXShaderPipelineCreateInfo *info);
void wLGFXDestroyShaderPipeline(LGFXShaderPipeline shaderPipeline);
void wLGFXUseShaderPipeline(LGFXCommandBuffer buffer, LGFXShaderPipeline shaderPipeline);

LGFXCommandBuffer wLGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
void wLGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
void wLGFXCommandBufferEnd(LGFXCommandBuffer buffer);
void wLGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
void wLGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain);
void wLGFXCommandBufferReset(LGFXCommandBuffer buffer);
void wLGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

void wLGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area);
void wLGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area);

void wLGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, size_t offset);
void wLGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, uint32_t vertexBuffersCount);
void wLGFXDrawIndexed(LGFXCommandBuffer commands, uint32_t indexCount, uint32_t instances, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
void wLGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, size_t bufferOffset, size_t drawCount, size_t drawParamsStride);

void wLGFXDispatchCompute(LGFXCommandBuffer commands, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ);
void wLGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, size_t offset);