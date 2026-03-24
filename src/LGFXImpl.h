#ifndef LGFXINSTANCE_H
#define LGFXINSTANCE_H

#include "lgfx/lgfx.h"
#include "lgfx/sync.h"

typedef struct LGFXInstanceImpl
{
    void *instance;
    bool runtimeErrorChecking;
    const char **enabledInstanceExtensions;
    const char **enabledErrorCheckerExtensions;
    LGFXBackendType backend;
} LGFXInstanceImpl;

typedef struct LGFXFunctionVariableBatchTemplateImpl
{
    void *handle;
    LGFXFunctionVariableCreateInfo *variables;
    uint32_t variablesCount;
} LGFXFunctionVariableBatchTemplateImpl;

typedef struct LGFXFencePool
{
	LGFXFence fences[LGFX_FENCE_POOL_SIZE];
    uint32_t numFences;
} LGFXFencePool;

LGFXFence LGFXFencePool_Rent(LGFXFencePool *pool, LGFXDevice device, bool initiallySignalled);
void LGFXFencePool_Return(LGFXFencePool *pool, LGFXFence fence);

typedef struct LGFXDeviceImpl
{
    LGFXInstance instance;

    void *physicalDevice;
    void *logicalDevice;
    void *memoryAllocator;
    void *descriptorPool;

    LGFXCommandQueue graphicsQueue;
    LGFXCommandQueue computeQueue;
    LGFXCommandQueue transferQueue;
    LGFXFencePool fencePool;

    LGFXBackendType backend;
} LGFXDeviceImpl;

typedef struct LGFXCommandQueueImpl
{
    void *queue;
    Lock queueLock;
    uint32_t queueFamilyID;

    void *regularCommandPool;
    void *transientCommandPool;
    Lock commandPoolLock;

    LGFXDevice inDevice;
    LGFXFence fence;
} LGFXCommandQueueImpl;

typedef struct LGFXSwapchainImpl
{
    bool invalidated;
    bool justCreated;

    void *windowSurface;

    void *swapchain;
    LGFXCreateWindowSurfaceFunc createSurfaceFunc;
    void *windowHandle;
    LGFXDevice device;
    uint32_t currentImageIndex;
    LGFXSwapchainPresentationMode presentMode;
    uint32_t width;
    uint32_t height;
    LGFXTexture *backbufferTextures;
    LGFXTexture *backDepthbuffers;
    uint32_t backbufferTexturesCount;

    LGFXFence fence;
    LGFXSemaphore awaitPresentComplete;
    LGFXSemaphore awaitRenderComplete;
} LGFXSwapchainImpl;

typedef struct LGFXFenceImpl
{
    void *fence;
    LGFXDevice device;
} LGFXFenceImpl;

typedef struct LGFXSemaphoreImpl
{
    void *semaphore;
    LGFXDevice device;
} LGFXSemaphoreImpl;

typedef struct LGFXCommandBufferImpl
{
    void *cmdBuffer;
    LGFXCommandQueue queue;
    bool begun;
} LGFXCommandBufferImpl;

typedef struct LGFXRenderProgramImpl
{
    void *handle;
    uint32_t currentPass;
    LGFXDevice device;

    LGFXRenderTarget *targets;
    uint32_t targetsCount;

    LGFXRenderTarget currentTarget;

    LGFXRenderAttachmentInfo *attachments;
    uint32_t attachmentsCount;

    bool outputToBackbuffer;
} LGFXRenderProgramImpl;

typedef struct LGFXFunctionImpl
{
    void *module1;
    void *module2;
    void *pipelineLayout;

    LGFXFunctionType type;
    LGFXDevice device;
} LGFXFunctionImpl;

typedef struct LGFXShaderPipelineImpl
{
    void *handle;
    LGFXFunction function;
    LGFXDevice device;
} LGFXShaderPipelineImpl;

typedef struct LGFXSamplerStateImpl
{
    void *handle;
    LGFXDevice device;
} LGFXSamplerStateImpl;

#endif