#ifndef LGFX_INSTANCE_H
#define LGFX_INSTANCE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdint.h>

typedef enum
{
    LGFXBackendType_Vulkan,
    LGFXBackendType_WebGPU
} LGFXBackendType;

typedef struct
{
    const char *appName;
    const char *engineName;
    uint32_t appVersion;
    uint32_t engineVersion;
    bool runtimeErrorChecking;
    LGFXBackendType backend;
} LGFXInstanceCreateInfo;

typedef struct
{
    /// @brief If true, multi draw indirect functions will be supported
    bool multiDrawIndirect;
    /// @brief If true, depth clamping will be supported by the GPU hardware
    bool depthClamp;
    /// @brief If true, depth bias clamping will be supported by the GPU hardware
    bool depthBiasClamp;
    /// @brief 
    bool fillModeNonSolid;
    /// @brief If true, rendering line-primitives with a width greater than a single pixel will be supported
    bool wideLines;
    /// @brief If true, ETC2 and EAC-compressed texture formats may be used
    bool textureCompressionETC2;
    /// @brief If true, ASTC-compressed texture formats may be used
    bool textureCompressionASTC;
    /// @brief If true, BC1 to 7 compressed texture formats may be used
    bool textureCompressionBC;
    /// @brief If true, the vertex stage in shaders may be used to write to storage images, buffers and atomic variables
    bool vertexShaderCanStoreDataAndAtomics;
    /// @brief If true, the fragment stage in shaders may be used to write to storage images, buffers and atomic variables
    bool fragmentShaderCanStoreDataAndAtomics;
    /// @brief If true, shaders can read and write to arrays of uniform buffers with a variable index. If false, they can only read from these arrays with a compile-time constant index value. Required for a technique known as bindless texturing.
    bool bindlessUniformBufferArrays;
    /// @brief If true, shaders can read and write to arrays of samplers and textures with a variable index. If false, they can only read from these arrays with a compile-time constant index value. Required for a technique known as bindless texturing.
    bool bindlessSamplerAndTextureArrays;
    /// @brief If true, shaders can read and write to arrays of storage (compute) buffers with a variable index. If false, they can only read from these arrays with a compile-time constant index value. Required for a technique known as bindless texturing.
    bool bindlessStorageBufferArrays;
    /// @brief If true, shaders can read and write to arrays of storage textures with a variable index. If false, they can only read from these arrays with a compile-time constant index value. Required for a technique known as bindless texturing.
    bool bindlessStorageTextureArrays;
    /// @brief If true, shaders can use 64-bit floats. If false, an error will be reported by the backend if such a usage occurs.
    bool shaderFloat64;
    /// @brief If true, shaders can use 64-bit integers. if false, an error will be reported by the backend if such a usage occurs.
    bool shaderInt64;
    /// @brief If true, shaders can use 16-bit integers. if false, an error will be reported by the backend if such a usage occurs.
    bool shaderInt16;
    /// @brief If true, resources can be sparsely bound and thus have empty unbound resources in between arrays of valid resources
    bool sparseBinding;
} LGFXDeviceFeatures;

typedef struct
{
    LGFXDeviceFeatures requiredFeatures;
} LGFXDeviceCreateInfo;

// typedef struct
// {
//     uint32_t maxTextureDimension1D;
//     uint32_t maxTextureDimension2D;
//     uint32_t maxTextureDimension3D;
// } LGFXGPULimits;

typedef struct LGFXInstanceImpl *LGFXInstance;
typedef struct LGFXDeviceImpl *LGFXDevice;
typedef struct LGFXSwapchainImpl *LGFXSwapchain;
typedef struct LGFXCommandQueueImpl *LGFXCommandQueue;
typedef struct LGFXFenceImpl *LGFXFence;
typedef struct LGFXSemaphoreImpl *LGFXSemaphore;

LGFXInstance LGFXCreateInstance(LGFXInstance instance, LGFXInstanceCreateInfo *info);

#endif