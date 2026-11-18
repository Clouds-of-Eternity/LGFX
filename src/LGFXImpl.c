#include "vulkan/LGFXVulkan.h"

LGFXInstance LGFXCreateInstance(const LGFXInstanceCreateInfo *info)
{
    if (info->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateInstance(info);
    }
    return NULL;
}
void LGFXDestroyInstance(LGFXInstance instance)
{
    if (instance->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyInstance(instance);
    }
}

LGFXFence LGFXCreateFence(LGFXDevice device, bool signalled)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFence(device, signalled);
    }
    return NULL;
}
void LGFXAwaitFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitFence(fence);
    }
}
void LGFXResetFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXResetFence(fence);
    }
}
void LGFXDestroyFence(LGFXFence fence)
{
    if (fence->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFence(fence);
    }
}

LGFXSemaphore LGFXCreateSemaphore(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSemaphore(device);
    }
    return NULL;
}
void LGFXDestroySemaphore(LGFXSemaphore semaphore)
{
    if (semaphore->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySemaphore(semaphore);
    }
}
LGFXSemaphore LGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetAwaitRenderedSemaphore(swapchain);
    }
    return NULL;
}
LGFXSemaphore LGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetAwaitPresentedSemaphore(swapchain);
    }
    return NULL;
}
uint32_t LGFXSwapchainGetCurrentFrameIndex(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetCurrentFrameIndex(swapchain);
    }
    return 0xFFFFFFFF;
}
uint32_t LGFXSwapchainGetCurrentImageIndex(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetCurrentImageIndex(swapchain);
    }
    return 0xFFFFFFFF;
}
LGFXTexture LGFXSwapchainGetCurrentFrame(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetCurrentFrame(swapchain);
    }
    return NULL;
}
LGFX_EXPORT LGFXTexture LGFXSwapchainGetCurrentImage(LGFXSwapchain swapchain)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXSwapchainGetCurrentImage(swapchain);
    }
    return NULL;
}

void LGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitWriteFunction(commandBuffer, funcType, opType);
    }
}
void LGFXAwaitDraw(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitDraw(commandBuffer);
    }
}
void LGFXAwaitGraphicsIdle(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXAwaitGraphicsIdle(device);
    }
}

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info)
{
    if (instance->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateDevice(instance, info);
    }
    return NULL;
}
void LGFXDestroyDevice(LGFXDevice device)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyDevice(device);
    }
}

LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSwapchain(device, info);
    }
    return NULL;
}

void LGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed)
{
    if (swapchain->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySwapchain(swapchain, windowIsDestroyed);
    }
}

LGFXTexture LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateTexture(device, info);
    }
    return NULL;
}
void LGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, uint32_t mipToTransition, uint32_t mipTransitionDepth)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXTextureTransitionLayout(device, texture, targetLayout, commandBuffer, mipToTransition, mipTransitionDepth);
    }
}
void LGFXTextureSetData(LGFXDevice device, LGFXTexture texture, uint8_t* bytes, size_t length)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXTextureSetData(device, texture, bytes, length);
    }
}
void LGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, uint32_t toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToTexture(device, commandBuffer, from, to, fromBufferOffset, toMip);
    }
}
void LGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, LGFXPoint3 extents, LGFXPoint3 offset, uint32_t toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToTextureWithExtents(device, commandBuffer, from, to, fromBufferOffset, extents, offset, toMip);
    }
}
void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, uint32_t toMip)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyTextureToBuffer(device, commandBuffer, from, to, toMip);
    }
}
void LGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, uint32_t fromMip, LGFXPoint3 toOffset, uint32_t toMip, LGFXPoint3 copyAreaSize, bool autoTransition)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyTextureToTexture(device, commandBuffer, from, to, fromOffset, fromMip, toOffset, toMip, copyAreaSize, autoTransition);
    }
}
void LGFXClearTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture texture, LGFXClearValues clearValues, uint32_t firstMipToTransition, uint32_t mipsToTransitionDepth, bool autoTransition)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXClearTexture(device, commandBuffer, texture, clearValues, firstMipToTransition, mipsToTransitionDepth, autoTransition);
    }
}
void LGFXDestroyTexture(LGFXTexture texture)
{
    if (texture->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyTexture(texture);
    }
}

LGFXSamplerState LGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateSamplerState(device, info);
    }
    return NULL;
}
void LGFXDestroySamplerState(LGFXSamplerState state)
{
    if (state->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroySamplerState(state);
    }
}

LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderTarget(device, info);
    }
    return NULL;
}
void LGFXDestroyRenderTarget(LGFXRenderTarget target)
{
    if (target->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyRenderTarget(target);
    }
}

LGFXBuffer LGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateBuffer(device, info);
    }
    return NULL;
}
void LGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to, size_t fromBufferOffset, size_t setIntoBufferOffset)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCopyBufferToBuffer(device, commandBuffer, from, to, fromBufferOffset, setIntoBufferOffset);
    }
}
void LGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetBufferDataOptimizedData(buffer, commandBufferToUse, data, setIntoBufferOffset, dataLength);
    }
}
void LGFXSetBufferDataFast(LGFXBuffer buffer, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetBufferDataFast(buffer, data, setIntoBufferOffset, dataLength);
    }
}
void LGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, uint32_t value)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXFillBuffer(cmdBuffer, buffer, value);
    }
}
void LGFXDestroyBuffer(LGFXBuffer buffer)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyBuffer(buffer);
    }
}
void *LGFXGetBufferData(LGFXBuffer buffer, size_t *bytesLength)
{
    if (bytesLength != NULL)
    {
        *bytesLength = buffer->size;
    }
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXGetBufferData(buffer);
    }
    return NULL;
}
bool LGFXBufferResize(LGFXBuffer buffer, size_t newSize)
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXBufferResize(buffer, newSize);
    }
    return false;
}
void *LGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(size_t))
{
    if (buffer->device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXReadBufferFromGPU(buffer, allocateFunction);
    }
    return NULL;
}

LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateRenderProgram(device, info);
    }
    return NULL;
}
void LGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXBeginRenderProgramSwapchain(program, commandBuffer, outputSwapchain, clearColor, autoTransitionTargetTextures);
    }
}
void LGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXBeginRenderProgram(program, commandBuffer, outputTarget, clearColor, autoTransitionTargetTextures);
    }
}
void LGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXRenderProgramNextPass(commandBuffer);
    }
}
void LGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXEndRenderProgram(program, commandBuffer);
    }
}
void LGFXDestroyRenderProgram(LGFXRenderProgram program)
{
    if (program->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyRenderProgram(program);
    }
}

LGFXFunctionVariableBatchTemplate LGFXCreateFunctionVariableBatchTemplate(LGFXDevice device, const LGFXFunctionVariableBatchTemplateCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableBatchTemplate(device, info);
    }
    return NULL;
}
LGFXFunctionVariableBatch LGFXCreateFunctionVariableBatchFromTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate fromTemplate)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableBatchFromTemplate(device, fromTemplate);
    }
    return NULL;
}
void LGFXDestroyFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate toDestroy)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFunctionVariableBatchTemplate(device, toDestroy);
    }
}

LGFXFunction LGFXCreateFunction(LGFXDevice device, const LGFXFunctionCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunction(device, info);
    }
    return NULL;
}
void LGFXDestroyFunction(LGFXFunction func)
{
    if (func->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyFunction(func);
    }
}

LGFXFunctionVariable LGFXCreateFunctionVariable(LGFXDevice device, LGFXFunctionVariableMetadata *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariable(device, info);
    }
    LGFXFunctionVariable empty = {0};
    return empty;
}
LGFXFunctionVariable LGFXCreateFunctionVariableSlot(LGFXDevice device, LGFXFunctionVariableBatchTemplate batchTemplate, uint32_t forVariableOfIndex)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateFunctionVariableSlot(device, batchTemplate, forVariableOfIndex);
    }
    LGFXFunctionVariable empty = {0};
    return empty;
}
void LGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *functionVariables, uint32_t variablesCount)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXFunctionSendVariablesToGPU(device, batch, functionVariables, variablesCount);
    }
}
void LGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction, uint32_t setIndex)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseFunctionVariables(commandBuffer, batch, forFunction, setIndex);
    }
}
void LGFXDestroyFunctionVariable(LGFXFunctionVariable variable)
{
	if (variable.valueIsOwnedBuffer)
	{
		LGFXBuffer buffer = (LGFXBuffer)variable.currentValues.asBuffers[0].buffer;
		LGFXDestroyBuffer(buffer);
	}
	free(variable.infos);
	free(variable.currentValues.asBuffers);
}

LGFXShaderPipeline LGFXCreateShaderPipeline(LGFXDevice device, LGFXShaderPipelineCreateInfo *info)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateShaderPipeline(device, info);
    }
    return NULL;
}
void LGFXDestroyShaderPipeline(LGFXShaderPipeline shaderPipeline)
{
    if (shaderPipeline->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyShaderPipeline(shaderPipeline);
    }
}
void LGFXUseShaderPipeline(LGFXCommandBuffer buffer, LGFXShaderPipeline shaderPipeline)
{
    if (shaderPipeline->device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseShaderPipeline(buffer, shaderPipeline);
    }
}


void LGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetViewport(commandBuffer, area);
    }
}
void LGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSetClipArea(commandBuffer, area);
    }
}

LGFXCommandBuffer LGFXCreateCommandBuffer(LGFXDevice device, bool forCompute)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXCreateCommandBuffer(device, forCompute);
    }
    return NULL;
}
void LGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferBegin(buffer, resetAfterSubmission);
    }
}
void LGFXCommandBufferEnd(LGFXCommandBuffer buffer)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferEnd(buffer);
    }
}
void LGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferExecute(buffer, fence, awaitSemaphore, signalSemaphore);
    }
}
void LGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferEndSwapchain(buffer, swapchain);
    }
}
void LGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer)
{
    if (commandBuffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDestroyCommandBuffer(commandBuffer);
    }
}
void LGFXCommandBufferReset(LGFXCommandBuffer buffer)
{
    if (buffer->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXCommandBufferReset(buffer);
    }
}

void LGFXUseIndexBuffer(LGFXCommandBuffer commands, const LGFXBuffer indexBuffer, size_t offset)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseIndexBuffer(commands, indexBuffer, offset);
    }
}
void LGFXUseVertexBuffer2(LGFXCommandBuffer commands, const LGFXBuffer *vertexBuffers, const size_t *offsetsPerVertexBuffer, uint32_t vertexBuffersCount)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXUseVertexBuffer(commands, vertexBuffers, offsetsPerVertexBuffer, vertexBuffersCount);
    }
}
void LGFXDrawIndexed(LGFXCommandBuffer commands, uint32_t indexCount, uint32_t instances, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDrawIndexed(commands, indexCount, instances, firstIndex, vertexOffset, firstInstance);
    }
}
void LGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, size_t bufferOffset, size_t drawCount, size_t drawParamsStride)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDrawIndexedIndirect(commands, drawParamsBuffer, bufferOffset, drawCount, drawParamsStride);
    }
}

void LGFXDispatchCompute(LGFXCommandBuffer commands, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDispatchCompute(commands, groupsX, groupsY, groupsZ);
    }
}
void LGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, size_t offset)
{
    if (commands->queue->inDevice->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXDispatchComputeIndirect(commands, dispatchParamsBuffer, offset);
    }
}

bool LGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, uint32_t frameWidth, uint32_t frameHeight)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        return VkLGFXNewFrame(device, swapchain, frameWidth, frameHeight);
    }
    return NULL;
}
void LGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain)
{
    if (device->backend == LGFXBackendType_Vulkan)
    {
        VkLGFXSubmitFrame(device, swapchain);
    }
}