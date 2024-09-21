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
    LGFXCommandQueue graphicsQueue;
    LGFXCommandQueue computeQueue;
    LGFXCommandQueue transferQueue;
} LGFXDeviceImpl;

typedef struct LGFXCommandQueueImpl
{
    void *queue;
    Mutex *queueMutex;

    LGFXDevice inDevice;
    LGFXFence fence;
} LGFXCommandQueueImpl;

typedef struct LGFXSwapchainImpl
{
    void *swapchain;
} LGFXSwapchainImpl;

typedef struct LGFXFenceImpl
{
    void *fence;
    LGFXDevice device;
} LGFXFenceImpl;

#endif