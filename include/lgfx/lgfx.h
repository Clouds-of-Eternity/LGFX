#ifndef LGFX_INSTANCE_H
#define LGFX_INSTANCE_H

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C"
{
#endif
#include "lgfx/Linxc.h"

typedef struct
{
    u8 R;
    u8 G;
    u8 B;
    u8 A;
} LGFXColor;

typedef struct
{
    i32 X;
    i32 Y;
    u32 width;
    u32 height;
} LGFXRectangle;

typedef struct
{
    float X;
    float Y;
    float width;
    float height;
} LGFXBox;

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
typedef struct LGFXSamplerStateImpl *LGFXSamplerState;
typedef struct LGFXFunctionVariableBatchImpl *LGFXFunctionVariableBatch;

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
    LGFXFunctionOperationType_IndexBufferRead = 1,
    LGFXFunctionOperationType_VertexBufferRead = 2,
    LGFXFunctionOperationType_IndirectBufferRead = 4,
    LGFXFunctionOperationType_ComputeBufferRead = 8,
    LGFXFunctionOperationType_UniformBufferRead = 16,
    LGFXFunctionOperationType_ComputeTextureRead = 32,
    LGFXFunctionOperationType_FragmentFunctionRead = 64
} LGFXFunctionOperationType;

typedef enum
{
    LGFXFilterType_Point,
    LGFXFilterType_Linear,
    LGFXFilterType_Cubic
} LGFXFilterType;

typedef enum
{
    LGFXSamplerRepeatMode_Repeat,
    LGFXSamplerRepeatMode_ClampToEdgeColor,
    LGFXSamplerRepeatMode_ClampToStaticColor
} LGFXSamplerRepeatMode;

typedef enum
{
    LGFXSamplerBorderColor_TransparentBlack,
    LGFXSamplerBorderColor_TransparentBlackInt,
    LGFXSamplerBorderColor_OpaqueBlack,
    LGFXSamplerBorderColor_OpaqueBlackInt,
    LGFXSamplerBorderColor_OpaqueWhite,
    LGFXSamplerBorderColor_OpaqueWhiteInt
} LGFXSamplerBorderColor;

typedef enum
{
    LGFXComparisonMode_Never,
    LGFXComparisonMode_Less,
    LGFXComparisonMode_Equal,
    LGFXComparisonMode_LessEqual,
    LGFXComparisonMode_Greater,
    LGFXComparisonMode_NotEqual,
    LGFXComparisonMode_GreaterEqual,
    LGFXComparisonMode_Always
} LGFXComparisonMode;

typedef enum
{
    LGFXFunctionType_Invalid = 0,
    LGFXFunctionType_Vertex = 1,
    LGFXFunctionType_Fragment = 2,
    LGFXFunctionType_Compute = 4,
    LGFXFunctionType_Tessellation = 8,
    LGFXFunctionType_Mesh = 16
} LGFXFunctionType;

typedef enum
{
    LGFXVertexElementFormat_Invalid,
    LGFXVertexElementFormat_Float,
    LGFXVertexElementFormat_Vector2,
    LGFXVertexElementFormat_Vector3,
    LGFXVertexElementFormat_Vector4,
    LGFXVertexElementFormat_Color,
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
    LGFXTextureFormat_Undefined,
    LGFXTextureFormat_R8Unorm,
    LGFXTextureFormat_R8Snorm,
    LGFXTextureFormat_R8Uint,
    LGFXTextureFormat_R8Sint,
    LGFXTextureFormat_R16Uint,
    LGFXTextureFormat_R16Sint,
    LGFXTextureFormat_R16Float,
    LGFXTextureFormat_RG8Unorm,
    LGFXTextureFormat_RG8Snorm,
    LGFXTextureFormat_RG8Uint,
    LGFXTextureFormat_RG8Sint,
    LGFXTextureFormat_R32Float,
    LGFXTextureFormat_R32Uint,
    LGFXTextureFormat_R32Sint,
    LGFXTextureFormat_RG16Uint,
    LGFXTextureFormat_RG16Sint,
    LGFXTextureFormat_RG16Float,
    LGFXTextureFormat_RGBA8Unorm,
    LGFXTextureFormat_RGBA8UnormSrgb,
    LGFXTextureFormat_RGBA8Snorm,
    LGFXTextureFormat_RGBA8Uint,
    LGFXTextureFormat_RGBA8Sint,
    LGFXTextureFormat_BGRA8Unorm,
    LGFXTextureFormat_BGRA8UnormSrgb,
    LGFXTextureFormat_RGB10A2Uint,
    LGFXTextureFormat_RGB10A2Unorm,
    LGFXTextureFormat_RG11B10Ufloat,
    LGFXTextureFormat_RGB9E5Ufloat,
    LGFXTextureFormat_RG32Float,
    LGFXTextureFormat_RG32Uint,
    LGFXTextureFormat_RG32Sint,
    LGFXTextureFormat_RGBA16Uint,
    LGFXTextureFormat_RGBA16Sint,
    LGFXTextureFormat_RGBA16Float,
    LGFXTextureFormat_RGBA32Float,
    LGFXTextureFormat_RGBA32Uint,
    LGFXTextureFormat_RGBA32Sint,
    LGFXTextureFormat_BC1RGBAUnorm,
    LGFXTextureFormat_BC1RGBAUnormSrgb,
    LGFXTextureFormat_BC2RGBAUnorm,
    LGFXTextureFormat_BC2RGBAUnormSrgb,
    LGFXTextureFormat_BC3RGBAUnorm,
    LGFXTextureFormat_BC3RGBAUnormSrgb,
    LGFXTextureFormat_BC4RUnorm,
    LGFXTextureFormat_BC4RSnorm,
    LGFXTextureFormat_BC5RGUnorm,
    LGFXTextureFormat_BC5RGSnorm,
    LGFXTextureFormat_BC6HRGBUfloat,
    LGFXTextureFormat_BC6HRGBFloat,
    LGFXTextureFormat_BC7RGBAUnorm,
    LGFXTextureFormat_BC7RGBAUnormSrgb,
    LGFXTextureFormat_ETC2RGB8Unorm,
    LGFXTextureFormat_ETC2RGB8UnormSrgb,
    LGFXTextureFormat_ETC2RGB8A1Unorm,
    LGFXTextureFormat_ETC2RGB8A1UnormSrgb,
    LGFXTextureFormat_ETC2RGBA8Unorm,
    LGFXTextureFormat_ETC2RGBA8UnormSrgb,
    LGFXTextureFormat_EACR11Unorm,
    LGFXTextureFormat_EACR11Snorm,
    LGFXTextureFormat_EACRG11Unorm,
    LGFXTextureFormat_EACRG11Snorm,
    LGFXTextureFormat_ASTC4x4Unorm,
    LGFXTextureFormat_ASTC4x4UnormSrgb,
    LGFXTextureFormat_ASTC5x4Unorm,
    LGFXTextureFormat_ASTC5x4UnormSrgb,
    LGFXTextureFormat_ASTC5x5Unorm,
    LGFXTextureFormat_ASTC5x5UnormSrgb,
    LGFXTextureFormat_ASTC6x5Unorm,
    LGFXTextureFormat_ASTC6x5UnormSrgb,
    LGFXTextureFormat_ASTC6x6Unorm,
    LGFXTextureFormat_ASTC6x6UnormSrgb,
    LGFXTextureFormat_ASTC8x5Unorm,
    LGFXTextureFormat_ASTC8x5UnormSrgb,
    LGFXTextureFormat_ASTC8x6Unorm,
    LGFXTextureFormat_ASTC8x6UnormSrgb,
    LGFXTextureFormat_ASTC8x8Unorm,
    LGFXTextureFormat_ASTC8x8UnormSrgb,
    LGFXTextureFormat_ASTC10x5Unorm,
    LGFXTextureFormat_ASTC10x5UnormSrgb,
    LGFXTextureFormat_ASTC10x6Unorm,
    LGFXTextureFormat_ASTC10x6UnormSrgb,
    LGFXTextureFormat_ASTC10x8Unorm,
    LGFXTextureFormat_ASTC10x8UnormSrgb,
    LGFXTextureFormat_ASTC10x10Unorm,
    LGFXTextureFormat_ASTC10x10UnormSrgb,
    LGFXTextureFormat_ASTC12x10Unorm,
    LGFXTextureFormat_ASTC12x10UnormSrgb,
    LGFXTextureFormat_ASTC12x12Unorm,
    LGFXTextureFormat_ASTC12x12UnormSrgb,
    LGFXTextureFormat_Stencil8,
    LGFXTextureFormat_Depth16Unorm,
    LGFXTextureFormat_Depth24Plus,
    LGFXTextureFormat_Depth24PlusStencil8,
    LGFXTextureFormat_Depth32Float,
    LGFXTextureFormat_Depth32FloatStencil8,
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
    u32 maxDescriptorSets;
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

typedef struct
{
    LGFXSamplerRepeatMode repeatModeU;
    LGFXSamplerRepeatMode repeatModeV;
    LGFXSamplerRepeatMode repeatModeW;
    LGFXFilterType magnificationFilter;
    LGFXFilterType minimizationFilter;
    LGFXFilterType mipmapLookupMode;
    float minLODClamp;
    float maxLODClamp;
    LGFXSamplerBorderColor borderColor;
    bool isAnisotropic;
    float maxAnisotropy;
    bool isComparisonSampler;
    LGFXComparisonMode comparisonOperation;
} LGFXSamplerStateCreateInfo;

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
    usize size;
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
    bool isTightlyPacked;
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
    void **currentValues;
    u32 valuesCount;
    void *infos;
    LGFXShaderResource *metadata;
    LGFXFunction forFunction;

    LGFXDevice device;
    bool valueIsOwnedBuffer;
    bool mutated;
} LGFXFunctionVariable;

typedef struct
{
    LGFXFunctionType type;

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
void LGFXAwaitFence(LGFXFence fence);
void LGFXResetFence(LGFXFence fence);
void LGFXDestroyFence(LGFXFence fence);

LGFXSemaphore LGFXCreateSemaphore(LGFXDevice device);
void LGFXDestroySemaphore(LGFXSemaphore semaphore);

void LGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType);
void LGFXAwaitDraw(LGFXCommandBuffer commandBuffer);
void LGFXAwaitGraphicsIdle();

LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
void LGFXDestroyDevice(LGFXDevice device);

LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
void LGFXAwaitSwapchainIdle(LGFXSwapchain swapchain);
void LGFXSwapchainInvalidate(LGFXSwapchain swapchain);
void LGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed);

LGFXTexture LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
void LGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth);
void LGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length);
void LGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip);
void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip);
void LGFXDestroyTexture(LGFXTexture texture);

LGFXSamplerState LGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info);
void LGFXDestroySamplerState(LGFXSamplerState state);

LGFXCommandBuffer LGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
void LGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
void LGFXCommandBufferEnd(LGFXCommandBuffer buffer);
void LGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
void LGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain);
void LGFXCommandBufferReset(LGFXCommandBuffer buffer);
void LGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
void LGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFXBuffer LGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
void LGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to);
void LGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, u8 *data, usize dataLength);
void LGFXSetBufferDataFast(LGFXBuffer buffer, u8 *data, usize dataLength);
void LGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, u32 value);
void LGFXDestroyBuffer(LGFXBuffer buffer);
void *LGFXGetBufferData(LGFXBuffer buffer, usize *bytesLength);
void *LGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(usize));

LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
void LGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures);
void LGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures);
void LGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer);
void LGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer);
void LGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFXFunction LGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info);
void LGFXDestroyFunction(LGFXFunction func);
LGFXFunctionVariableBatch LGFXFunctionGetVariableBatch(LGFXFunction function);
LGFXFunctionVariable LGFXFunctionGetVariableSlot(LGFXFunction function, u32 forVariableOfIndex);
void LGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *functionVariables, u32 variablesCount);
void LGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction);
void LGFXDestroyFunctionVariable(LGFXFunctionVariable variable);

LGFXShaderState LGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info);
void LGFXDestroyShaderState(LGFXShaderState shaderState);
void LGFXUseShaderState(LGFXCommandBuffer buffer, LGFXShaderState shaderState);

void LGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area);
void LGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area);

void LGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, usize offset);
void LGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, u32 vertexBuffersCount);
void LGFXDrawIndexed(LGFXCommandBuffer commands, u32 indexCount, u32 instances, u32 firstIndex, u32 vertexOffset, u32 firstInstance);
void LGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, usize bufferOffset, usize drawCount, usize drawParamsStride);

void LGFXDispatchCompute(LGFXCommandBuffer commands, u32 groupsX, u32 groupsY, u32 groupsZ);
void LGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, usize offset);

bool LGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight);
void LGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain);

#ifdef __cplusplus
}
#endif

#endif