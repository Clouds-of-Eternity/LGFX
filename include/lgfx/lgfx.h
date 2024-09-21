#ifndef LGFX_INSTANCE_H
#define LGFX_INSTANCE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdint.h>

typedef struct LGFXInstanceImpl *LGFXInstance;
typedef struct LGFXDeviceImpl *LGFXDevice;
typedef struct LGFXSwapchainImpl *LGFXSwapchain;
typedef struct LGFXCommandQueueImpl *LGFXCommandQueue;
typedef struct LGFXFenceImpl *LGFXFence;
typedef struct LGFXSemaphoreImpl *LGFXSemaphore;
typedef struct LGFXMemoryBlockImpl *LGFXMemoryBlock;

typedef enum
{
    LGFXBackendType_Vulkan,
    LGFXBackendType_WebGPU
} LGFXBackendType;

typedef enum
{
    LGFXSwapchainPresentationMode_Immediate = 0,
    LGFXSwapchainPresentationMode_Mailbox = 1,
    LGFXSwapchainPresentationMode_Fifo = 2
} LGFXSwapchainPresentationMode;

typedef enum
{
    LGFXTextureLayout_Undefined,
    LGFXTextureLayout_General,
    LGFXTextureLayout_ColorAttachmentOptimal,
    LGFXTextureLayout_DepthStencilAttachmentOptimal,
    LGFXTextureLayout_DepthStencilReadOptimal,
    LGFXTextureLayout_ShaderReadOptimal,
    LGFXTextureLayout_TransferSrcOptimal,
    LGFXTextureLayout_TransferDstOptimal,
    LGFXTextureLayout_Preinitialized,
    LGFXTextureLayout_PresentSource,
    LGFXTextureLayout_SharedPresentSrc,
    LGFXTextureLayout_FragmentDensityMap,
    LGFXTextureLayout_FragmentShadingRateAttachmentOptimal
} LGFXTextureLayout;

typedef enum
{
    LGFXTextureUsage_TransferSource = 0x00000001,
    LGFXTextureUsage_TransferDest = 0x00000002,
    LGFXTextureUsage_Sampled = 0x00000004,
    LGFXTextureUsage_Storage = 0x00000008,
    LGFXTextureUsage_ColorAttachment = 0x00000010,
    LGFXTextureUsage_DepthAttachment = 0x00000020,
    LGFXTextureUsage_TransientAttachment = 0x00000040,
    LGFXTextureUsage_InputAttachment = 0x00000080,
    LGFXTextureUsage_FragmentShadingRateAttachment = 0x00000100,
    LGFXTextureUsage_FragmentDensityMap = 0x00000200,
} LGFXTextureUsage;

typedef enum
{
    LGFXTextureFormat_Undefined = 0x00000000,
    LGFXTextureFormat_R8Unorm = 0x00000001,
    LGFXTextureFormat_R8Snorm = 0x00000002,
    LGFXTextureFormat_R8Uint = 0x00000003,
    LGFXTextureFormat_R8Sint = 0x00000004,
    LGFXTextureFormat_R16Uint = 0x00000005,
    LGFXTextureFormat_R16Sint = 0x00000006,
    LGFXTextureFormat_R16Float = 0x00000007,
    LGFXTextureFormat_RG8Unorm = 0x00000008,
    LGFXTextureFormat_RG8Snorm = 0x00000009,
    LGFXTextureFormat_RG8Uint = 0x0000000A,
    LGFXTextureFormat_RG8Sint = 0x0000000B,
    LGFXTextureFormat_R32Float = 0x0000000C,
    LGFXTextureFormat_R32Uint = 0x0000000D,
    LGFXTextureFormat_R32Sint = 0x0000000E,
    LGFXTextureFormat_RG16Uint = 0x0000000F,
    LGFXTextureFormat_RG16Sint = 0x00000010,
    LGFXTextureFormat_RG16Float = 0x00000011,
    LGFXTextureFormat_RGBA8Unorm = 0x00000012,
    LGFXTextureFormat_RGBA8UnormSrgb = 0x00000013,
    LGFXTextureFormat_RGBA8Snorm = 0x00000014,
    LGFXTextureFormat_RGBA8Uint = 0x00000015,
    LGFXTextureFormat_RGBA8Sint = 0x00000016,
    LGFXTextureFormat_BGRA8Unorm = 0x00000017,
    LGFXTextureFormat_BGRA8UnormSrgb = 0x00000018,
    LGFXTextureFormat_RGB10A2Uint = 0x00000019,
    LGFXTextureFormat_RGB10A2Unorm = 0x0000001A,
    LGFXTextureFormat_RG11B10Ufloat = 0x0000001B,
    LGFXTextureFormat_RGB9E5Ufloat = 0x0000001C,
    LGFXTextureFormat_RG32Float = 0x0000001D,
    LGFXTextureFormat_RG32Uint = 0x0000001E,
    LGFXTextureFormat_RG32Sint = 0x0000001F,
    LGFXTextureFormat_RGBA16Uint = 0x00000020,
    LGFXTextureFormat_RGBA16Sint = 0x00000021,
    LGFXTextureFormat_RGBA16Float = 0x00000022,
    LGFXTextureFormat_RGBA32Float = 0x00000023,
    LGFXTextureFormat_RGBA32Uint = 0x00000024,
    LGFXTextureFormat_RGBA32Sint = 0x00000025,
    LGFXTextureFormat_Stencil8 = 0x00000026,
    LGFXTextureFormat_Depth16Unorm = 0x00000027,
    LGFXTextureFormat_Depth24Plus = 0x00000028,
    LGFXTextureFormat_Depth24PlusStencil8 = 0x00000029,
    LGFXTextureFormat_Depth32Float = 0x0000002A,
    LGFXTextureFormat_Depth32FloatStencil8 = 0x0000002B,
    LGFXTextureFormat_BC1RGBAUnorm = 0x0000002C,
    LGFXTextureFormat_BC1RGBAUnormSrgb = 0x0000002D,
    LGFXTextureFormat_BC2RGBAUnorm = 0x0000002E,
    LGFXTextureFormat_BC2RGBAUnormSrgb = 0x0000002F,
    LGFXTextureFormat_BC3RGBAUnorm = 0x00000030,
    LGFXTextureFormat_BC3RGBAUnormSrgb = 0x00000031,
    LGFXTextureFormat_BC4RUnorm = 0x00000032,
    LGFXTextureFormat_BC4RSnorm = 0x00000033,
    LGFXTextureFormat_BC5RGUnorm = 0x00000034,
    LGFXTextureFormat_BC5RGSnorm = 0x00000035,
    LGFXTextureFormat_BC6HRGBUfloat = 0x00000036,
    LGFXTextureFormat_BC6HRGBFloat = 0x00000037,
    LGFXTextureFormat_BC7RGBAUnorm = 0x00000038,
    LGFXTextureFormat_BC7RGBAUnormSrgb = 0x00000039,
    LGFXTextureFormat_ETC2RGB8Unorm = 0x0000003A,
    LGFXTextureFormat_ETC2RGB8UnormSrgb = 0x0000003B,
    LGFXTextureFormat_ETC2RGB8A1Unorm = 0x0000003C,
    LGFXTextureFormat_ETC2RGB8A1UnormSrgb = 0x0000003D,
    LGFXTextureFormat_ETC2RGBA8Unorm = 0x0000003E,
    LGFXTextureFormat_ETC2RGBA8UnormSrgb = 0x0000003F,
    LGFXTextureFormat_EACR11Unorm = 0x00000040,
    LGFXTextureFormat_EACR11Snorm = 0x00000041,
    LGFXTextureFormat_EACRG11Unorm = 0x00000042,
    LGFXTextureFormat_EACRG11Snorm = 0x00000043,
    LGFXTextureFormat_ASTC4x4Unorm = 0x00000044,
    LGFXTextureFormat_ASTC4x4UnormSrgb = 0x00000045,
    LGFXTextureFormat_ASTC5x4Unorm = 0x00000046,
    LGFXTextureFormat_ASTC5x4UnormSrgb = 0x00000047,
    LGFXTextureFormat_ASTC5x5Unorm = 0x00000048,
    LGFXTextureFormat_ASTC5x5UnormSrgb = 0x00000049,
    LGFXTextureFormat_ASTC6x5Unorm = 0x0000004A,
    LGFXTextureFormat_ASTC6x5UnormSrgb = 0x0000004B,
    LGFXTextureFormat_ASTC6x6Unorm = 0x0000004C,
    LGFXTextureFormat_ASTC6x6UnormSrgb = 0x0000004D,
    LGFXTextureFormat_ASTC8x5Unorm = 0x0000004E,
    LGFXTextureFormat_ASTC8x5UnormSrgb = 0x0000004F,
    LGFXTextureFormat_ASTC8x6Unorm = 0x00000050,
    LGFXTextureFormat_ASTC8x6UnormSrgb = 0x00000051,
    LGFXTextureFormat_ASTC8x8Unorm = 0x00000052,
    LGFXTextureFormat_ASTC8x8UnormSrgb = 0x00000053,
    LGFXTextureFormat_ASTC10x5Unorm = 0x00000054,
    LGFXTextureFormat_ASTC10x5UnormSrgb = 0x00000055,
    LGFXTextureFormat_ASTC10x6Unorm = 0x00000056,
    LGFXTextureFormat_ASTC10x6UnormSrgb = 0x00000057,
    LGFXTextureFormat_ASTC10x8Unorm = 0x00000058,
    LGFXTextureFormat_ASTC10x8UnormSrgb = 0x00000059,
    LGFXTextureFormat_ASTC10x10Unorm = 0x0000005A,
    LGFXTextureFormat_ASTC10x10UnormSrgb = 0x0000005B,
    LGFXTextureFormat_ASTC12x10Unorm = 0x0000005C,
    LGFXTextureFormat_ASTC12x10UnormSrgb = 0x0000005D,
    LGFXTextureFormat_ASTC12x12Unorm = 0x0000005E,
    LGFXTextureFormat_ASTC12x12UnormSrgb = 0x0000005F,
} LGFXTextureFormat;

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
    void *windowSurface;
    LGFXSwapchainPresentationMode presentationMode;
    u32 width;
    u32 height;
    LGFXSwapchain oldSwapchain
} LGFXSwapchainCreateInfo;

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

typedef struct
{
    u8 *pixelData;
    LGFXTextureFormat format;
    u32 mipLevels;
    u32 width;
    u32 height;
} LGFXTextureCreateInfo;

typedef struct
{
    void *imageHandle;
    void *imageView;
    LGFXTextureLayout layout;
    u32 width;
    u32 height;
    u32 mipLevels;
    LGFXMemoryBlock textureMemory;
} LGFXTexture2D;

LGFXTexture2D LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);

LGFXInstance LGFXCreateInstance(LGFXInstance instance, LGFXInstanceCreateInfo *info);
void LGFXDestroyInstance(LGFXInstance instance);

LGFXFence LGFXCreateFence(LGFXDevice device, bool signalled);
void LGFXDestroyFence(LGFXFence fence);

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void LGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void LGFXDestroySwapcahin(LGFXSwapchain swapchain);

#endif