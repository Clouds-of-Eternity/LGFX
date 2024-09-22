#ifndef LGFXINSTANCE_HPP
#define LGFXINSTANCE_HPP

#include "lgfx/lgfx.h"
#include "Linxc.h"

typedef struct Mutex Mutex;

typedef struct LGFXInstanceImpl
{
    void *instance;
    bool runtimeErrorChecking;
    const char **enabledInstanceExtensions;
    const char **enabledErrorCheckerExtensions;
} LGFXInstanceImpl;

typedef struct LGFXDeviceImpl
{
    void *physicalDevice;
    void *logicalDevice;
    void *memoryAllocator;
    LGFXCommandQueue graphicsQueue;
    LGFXCommandQueue computeQueue;
    LGFXCommandQueue transferQueue;
} LGFXDeviceImpl;

typedef struct LGFXCommandQueueImpl
{
    void *queue;
    Mutex *queueMutex;
    u32 queueFamilyID;

    void *transientCommandPool;
    Mutex *commandPoolMutex;

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

#endif