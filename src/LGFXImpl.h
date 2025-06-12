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

typedef struct LGFXFencePool
{
	LGFXFence fences[LGFX_FENCE_POOL_SIZE];
    u32 numFences;
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
    u32 queueFamilyID;

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

    void *swapchain;
    void *nativeWindowHandle;
    void *windowSurface;
    LGFXDevice device;
    u32 currentImageIndex;
    LGFXSwapchainPresentationMode presentMode;
    u32 width;
    u32 height;
    LGFXTexture *backbufferTextures;
    LGFXTexture *backDepthbuffers;
    u32 backbufferTexturesCount;

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
    u32 currentPass;
    LGFXDevice device;

    LGFXRenderTarget *targets;
    u32 targetsCount;

    LGFXRenderTarget currentTarget;

    LGFXRenderAttachmentInfo *attachments;
    u32 attachmentsCount;

    bool outputToBackbuffer;
} LGFXRenderProgramImpl;

typedef struct LGFXFunctionImpl
{
    void *module1;
    void *module2;
    void *functionVariablesLayout;
    void *pipelineLayout;

    LGFXFunctionType type;

    LGFXShaderResource *uniforms;
    u32 uniformsCount;
    LGFXDevice device;
} LGFXFunctionImpl;

typedef struct LGFXShaderStateImpl
{
    void *handle;
    LGFXFunction function;
    LGFXDevice device;
} LGFXShaderStateImpl;

typedef struct LGFXSamplerStateImpl
{
    void *handle;
    LGFXDevice device;
} LGFXSamplerStateImpl;

#endif