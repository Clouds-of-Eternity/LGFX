#ifndef LGFXINSTANCE_H
#define LGFXINSTANCE_H

#include "lgfx/lgfx.h"
#include "sync.h"

typedef struct LGFXInstanceImpl
{
    void *instance;
    bool runtimeErrorChecking;
    const char **enabledInstanceExtensions;
    const char **enabledErrorCheckerExtensions;
    LGFXBackendType backend;
} LGFXInstanceImpl;

typedef struct LGFXDeviceImpl
{
    void *physicalDevice;
    void *logicalDevice;
    void *memoryAllocator;
    LGFXCommandQueue graphicsQueue;
    LGFXCommandQueue computeQueue;
    LGFXCommandQueue transferQueue;

    LGFXBackendType backend;
} LGFXDeviceImpl;

typedef struct LGFXCommandQueueImpl
{
    void *queue;
    Lock queueLock;
    u32 queueFamilyID;

    void *transientCommandPool;
    Lock commandPoolLock;

    LGFXDevice inDevice;
    LGFXFence fence;
} LGFXCommandQueueImpl;

typedef struct LGFXSwapchainImpl
{
    void *swapchain;
    void **images;
    void *windowSurface;
    LGFXDevice device;
    u32 currentImageIndex;
    LGFXSwapchainPresentationMode presentMode;
    u32 width;
    u32 height;
    u32 imageCount;
} LGFXSwapchainImpl;

typedef struct LGFXFenceImpl
{
    void *fence;
    LGFXDevice device;
} LGFXFenceImpl;

typedef struct LGFXCommandBufferImpl
{
    void *cmdBuffer;
    LGFXCommandQueue queue;
    bool begun;
} LGFXCommandBufferImpl;

typedef struct RenderProgramImageAttachment
{
    /// The format of the image that the render stage(s) with this associated attachment should take
    LGFXTextureFormat textureFormat;
    /// Whether the attached image should be cleared when beginning the pass. If transparent, no clearing is performed
    bool clearColor;
    /// Whether the attached depth buffer should be cleared when beginning the pass. Not applicable if depthAttachmentIndex is -1
    bool clearDepth;
    /// Which renderprogram this pass belongs to
    LGFXRenderProgram inProgram;
} RenderProgramImageAttachment;

typedef struct LGFXRenderProgramImpl
{
    void *handle;
    u32 currentPass;
    LGFXDevice device;

    LGFXRenderTarget *targets;
    u32 targetsCount;
} LGFXRenderProgramImpl;

typedef struct LGFXFunctionImpl
{
    void *module1;
    void *module2;
    void *functionVariablesLayout;

    LGFXShaderResource *uniforms;
    u32 uniformsCount;
    LGFXDevice device;
} LGFXFunctionImpl;

typedef struct LGFXShaderStateImpl
{
    void *handle;
    void *pipelineLayoutHandle;
    LGFXFunction function;
    LGFXDevice device;
} LGFXShaderStateImpl;

#endif