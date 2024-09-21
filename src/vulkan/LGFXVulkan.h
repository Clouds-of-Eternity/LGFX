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

#endif