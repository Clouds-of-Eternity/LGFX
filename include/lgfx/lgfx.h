#ifndef LGFX_INSTANCE_H
#define LGFX_INSTANCE_H

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C"
{
#endif
#include "lgfx/Linxc.h"

typedef struct LGFXInstanceImpl *LGFXInstance;
typedef struct LGFXDeviceImpl *LGFXDevice;
typedef struct LGFXSwapchainImpl *LGFXSwapchain;
typedef struct LGFXCommandQueueImpl *LGFXCommandQueue;
typedef struct LGFXFenceImpl *LGFXFence;
typedef struct LGFXSemaphoreImpl *LGFXSemaphore;
typedef struct LGFXMemoryBlockImpl *LGFXMemoryBlock;
typedef struct LGFXCommandBufferImpl *LGFXCommandBuffer;
typedef struct LGFXBufferImpl *LGFXBuffer;
typedef struct LGFXTextureImpl *LGFXTexture;
typedef struct LGFXRenderTargetImpl *LGFXRenderTarget;
typedef struct LGFXRenderProgramImpl *LGFXRenderProgram;
typedef struct LGFXFunctionImpl *LGFXFunction;
typedef struct LGFXShaderStateImpl *LGFXShaderState;
typedef struct LGFXComputeImpl *LGFXCompute;

typedef enum
{
    LGFXBackendType_Vulkan,
    LGFXBackendType_WebGPU
} LGFXBackendType;

typedef enum
{
    LGFXShaderResourceType_Uniform,
    LGFXShaderResourceType_Sampler,
    LGFXShaderResourceType_Texture,
    LGFXShaderResourceType_StructuredBuffer,
    LGFXShaderResourceType_InputAttachment,
    LGFXShaderResourceType_StorageTexture
} LGFXShaderResourceType;

typedef enum
{
    LGFXVertexElementFormat_Invalid,
    LGFXVertexElementFormat_Float,
    LGFXVertexElementFormat_Vector2,
    LGFXVertexElementFormat_Vector3,
    LGFXVertexElementFormat_Vector4,
    LGFXVertexElementFormat_Int,
    LGFXVertexElementFormat_Uint
} LGFXVertexElementFormat;

typedef enum
{
    LGFXBlend_Disable,
    LGFXBlend_One,
    LGFXBlend_Zero,
    LGFXBlend_SourceColor,
    LGFXBlend_InverseSourceColor,
    LGFXBlend_SourceAlpha,
    LGFXBlend_InverseSourceAlpha,
    LGFXBlend_DestinationColor,
    LGFXBlend_InverseDestinationColor,
    LGFXBlend_DestinationAlpha,
    LGFXBlend_InverseDestinationAlpha
} LGFXBlend;

typedef enum
{
    LGFXShaderInputAccess_Vertex = 1,
    LGFXShaderInputAccess_Fragment = 2,
    LGFXShaderInputAccess_Compute = 4
} LGFXShaderInputAccessFlags;

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
    /// Renders the specified vertices as a sequence of isolated triangles. Each group of three vertices defines a separate triangle.
    LGFXPrimitiveType_TriangleList,

    /// Renders the vertices as a triangle strip.
    LGFXPrimitiveType_TriangleStrip,

    /// Renders the vertices as a list of isolated straight line segments; the count may be any positive integer.
    LGFXPrimitiveType_LineList,

    /// Renders the vertices as a single polyline; the count may be any positive integer.
    LGFXPrimitiveType_LineStrip,

    // Renders the vertices as individual points; the count may be any positive integer.
    LGFXPrimitiveType_PointList,

    /// Renders the vertices as a series of triangle primitives connected to a central origin vertex
    LGFXPrimitiveType_TriangleFan,
} LGFXPrimitiveType;

typedef enum
{
    LGFXBufferUsage_TransferSource = 0x00000001,
    LGFXBufferUsage_TransferDest = 0x00000002,
    LGFXBufferUsage_UniformTexel = 0x00000004,
    LGFXBufferUsage_StorageTexel = 0x00000008,
    LGFXBufferUsage_UniformBuffer = 0x00000010,
    LGFXBufferUsage_StorageBuffer = 0x00000020,
    LGFXBufferUsage_IndexBuffer = 0x00000040,
    LGFXBufferUsage_VertexBuffer = 0x00000080,
    LGFXBufferUsage_IndirectDrawCallBuffer = 0x00000100,
} LGFXBufferUsage;

typedef enum
{
    LGFXCullMode_None,
    LGFXCullMode_Front,
    LGFXCullMode_Back
} LGFXCullMode;

typedef enum
{
    LGFXMemoryUsage_CPU_TO_GPU,
    LGFXMemoryUsage_GPU_TO_CPU,
    LGFXMemoryUsage_GPU_ONLY
} LGFXMemoryUsage;

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
    u32 appVersion;
    u32 engineVersion;
    bool runtimeErrorChecking;
    const char **enabledExtensions;
    u32 enabledExtensionsCount;
    LGFXBackendType backend;
} LGFXInstanceCreateInfo;

typedef struct
{
    void *nativeWindowHandle;
    LGFXSwapchainPresentationMode presentationMode;
    u32 width;
    u32 height;
    LGFXSwapchain oldSwapchain;
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
    LGFXTextureFormat format;
    LGFXTextureUsage usage;
    u32 mipLevels;
    u32 width;
    u32 height;
    u32 depth;
    u32 sampleCount;

    void *externalTextureHandle;
} LGFXTextureCreateInfo;

typedef struct LGFXTextureImpl
{
    void *imageHandle;
    void *imageView;
    bool ownsHandle;
    LGFXTextureFormat format;
    LGFXTextureLayout layout;
    u32 width;
    u32 height;
    u32 depth;
    u32 mipLevels;
    u32 sampleCount;
    LGFXMemoryBlock textureMemory;
    LGFXDevice device;
} LGFXTextureImpl;

typedef struct
{
    usize size;
    LGFXBufferUsage bufferUsage;
    LGFXMemoryUsage memoryUsage;
} LGFXBufferCreateInfo;
typedef struct LGFXBufferImpl
{
    void *handle;
    LGFXBufferUsage usage;
    LGFXMemoryBlock bufferMemory;
    LGFXDevice device;
} LGFXBufferImpl;

typedef struct
{
    LGFXTexture *textures;
    u32 texturesCount;
    LGFXRenderProgram forRenderProgram;
} LGFXRenderTargetCreateInfo;
typedef struct LGFXRenderTargetImpl
{
    void *handle;
    LGFXTexture *textures;
    u32 texturesCount;
    LGFXDevice device;
} LGFXRenderTargetImpl;

/// @brief An attachment refers to the state of the textures passing in and out of each renderpass
typedef struct
{
    LGFXTextureFormat format;
    bool clear;
    bool readByRenderTarget;
} LGFXRenderAttachmentInfo;
/// @brief A pass is a stage of the render program. Currently, all passes in a program execute sequentially, and depend on the completion of the previous pass to execute.
typedef struct
{
    i32 *colorAttachmentIDs;
    u32 colorAttachmentsCount;
    i32 depthAttachmentID;

    i32 *readAttachmentIDs;
    u32 readAttachmentsCount;
} LGFXRenderPassInfo;
typedef struct
{
    bool outputToBackbuffer;
    LGFXRenderAttachmentInfo *attachments;
    u32 attachmentsCount;

    LGFXRenderPassInfo *renderPasses;
    u32 renderPassCount;
} LGFXRenderProgramCreateInfo;

typedef struct
{
    LGFXVertexElementFormat format;
    u32 offset;
} LGFXVertexAttribute;
typedef struct
{
    LGFXVertexAttribute *elements;
    u32 elementsCount;
    bool isPerInstance;
    u32 packedSize;
} LGFXVertexDeclaration;
LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, u32 elementsCount, bool isPerInstance, bool tightlyPacked);

typedef struct LGFXBlendState
{
    LGFXBlend sourceColorBlend;
    LGFXBlend sourceAlphaBlend;
    LGFXBlend destinationColorBlend;
    LGFXBlend destinationAlphaBlend;
} LGFXBlendState;

inline bool LGFXBlendStateEquals(const LGFXBlendState left, LGFXBlendState right)
{
    return left.sourceAlphaBlend == right.sourceAlphaBlend
        && left.sourceColorBlend == right.sourceColorBlend
        && left.destinationAlphaBlend == right.destinationAlphaBlend
        && left.destinationColorBlend == right.destinationColorBlend;
}

#define DISABLE_BLEND {LGFXBlend_Disable, LGFXBlend_Disable, LGFXBlend_Disable, LGFXBlend_Disable}
#define ADDITIVE_BLEND {LGFXBlend_SourceAlpha, LGFXBlend_SourceAlpha, LGFXBlend_One, LGFXBlend_One}
#define ALPHA_BLEND {LGFXBlend_One, LGFXBlend_One, LGFXBlend_InverseSourceAlpha, LGFXBlend_InverseSourceAlpha}
#define NON_PREMULTIPLIED_BLEND {LGFXBlend_SourceAlpha, LGFXBlend_SourceAlpha, LGFXBlend_InverseSourceAlpha, LGFXBlend_InverseSourceAlpha}
#define OPAQUE_BLEND {LGFXBlend_One, LGFXBlend_One, LGFXBlend_Zero, LGFXBlend_Zero}

typedef struct LGFXShaderResource
{
    const char* variableName;
    LGFXShaderResourceType type;
    u32 set;
    u32 binding;
    u32 arrayLength;
    u32 inputAttachmentIndex;
    u32 size;
    LGFXShaderInputAccessFlags accessedBy;
} LGFXShaderResource;
typedef struct
{
    u32 *module1Data;
    usize module1DataLength;

    u32 *module2Data;
    usize module2DataLength;

    LGFXShaderResource *uniforms;
    u32 uniformsCount;
    
} LGFXFunctionCreateInfo;
typedef struct LGFXShaderStateCreateInfo
{
    LGFXFunction function;
    bool dynamicViewportScissor;
    bool dynamicLineWidth;
    bool depthTest;
    bool depthWrite;

    LGFXVertexDeclaration *vertexDeclarations;
    u32 vertexDeclarationCount;

    LGFXPrimitiveType primitiveType;
    LGFXCullMode cullMode;
    LGFXBlendState blendState;

    LGFXRenderProgram forRenderProgram;
    u32 forRenderPass;
} LGFXShaderStateCreateInfo;

LGFXInstance LGFXCreateInstance(LGFXInstanceCreateInfo *info);
void LGFXDestroyInstance(LGFXInstance instance);

LGFXFence LGFXCreateFence(LGFXDevice device, bool signalled);
void LGFXDestroyFence(LGFXFence fence);

LGFXSemaphore LGFXCreateSemaphore(LGFXDevice device);
void LGFXDestroySemaphore(LGFXSemaphore semaphore);

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void LGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void LGFXDestroySwapchain(LGFXSwapchain swapchain);

LGFXTexture LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void LGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth);
void LGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length);
void LGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip);
void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip);
void LGFXDestroyTexture(LGFXTexture texture);

LGFXCommandBuffer LGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
void LGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
void LGFXCommandBufferEnd(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
void LGFXCommandBufferReset(LGFXCommandBuffer buffer);
void LGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void LGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer LGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void LGFXDestroyBuffer(LGFXBuffer buffer);

LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void LGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXFunction LGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info);
void LGFXDestroyFunction(LGFXFunction func);

LGFXShaderState LGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info);
void LGFXDestroyShaderState(LGFXShaderState shaderState);

bool LGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight);
void LGFXSubmitFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight);

#ifdef __cplusplus
}
#endif

#endif