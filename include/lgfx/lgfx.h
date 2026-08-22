#pragma once

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C"
{
#endif

#include <stdint.h>
#ifdef MACOS
#include <sys/types.h>
#endif

#if defined(_MSC_VER) && defined(BUILD_LGFX_DLL)
//Compile with Visual Studio, build as DLL
#define LGFX_EXPORT __declspec(dllexport)
#elif defined(_MSC_VER) && defined(USE_LGFX_DLL)
//Compile with Visual Studio, use as DLL
#define LGFX_EXPORT __declspec(dllimport)
#elif defined(USE_LGFX_DLL) || defined(BUILD_LGFX_DLL)
//Compile with clang/gcc, build or use as DLL
#define LGFX_EXPORT __attribute__((visibility("default")))
#else
//build as static library
#define LGFX_EXPORT
#endif

typedef struct LGFXColor
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
} LGFXColor;

typedef struct LGFXRectangle
{
    int32_t X;
    int32_t Y;
    uint32_t width;
    uint32_t height;
} LGFXRectangle;

typedef struct LGFXPoint3
{
    int32_t X;
    int32_t Y;
    int32_t Z;
} LGFXPoint3;

typedef struct LGFXBox
{
    float X;
    float Y;
    float width;
    float height;
} LGFXBox;

typedef union LGFXClearValues
{
    float floatRGBA[4];
    int32_t intRGBA[4];
    uint32_t uintRGBA[4];
} LGFXClearValues;

typedef struct LGFXRenderAttachmentInfo LGFXRenderAttachmentInfo;
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
typedef struct LGFXShaderPipelineImpl *LGFXShaderPipeline;
typedef struct LGFXComputeImpl *LGFXCompute;
typedef struct LGFXSamplerStateImpl *LGFXSamplerState;
typedef struct LGFXFunctionVariableBatchImpl *LGFXFunctionVariableBatch;
typedef struct LGFXFunctionVariableBatchTemplateImpl *LGFXFunctionVariableBatchTemplate;

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
    LGFXFunctionType_Mesh = 16,
    LGFXFunctionType_Unknown = 0xFFFFFFFF
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
    LGFXVertexElementFormat_Int2,
    LGFXVertexElementFormat_Int3,
    LGFXVertexElementFormat_Int4,
    LGFXVertexElementFormat_UInt,
    LGFXVertexElementFormat_UInt2,
    LGFXVertexElementFormat_UInt3,
    LGFXVertexElementFormat_UInt4
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
    LGFXRenderAttachmentOutput_ToRenderTarget,
    LGFXRenderAttachmentOutput_ToNextPass,
    LGFXRenderAttachmentOutput_ToScreen
} LGFXRenderAttachmentOutput;

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

typedef struct LGFXInstanceCreateInfo
{
    /// @brief The reference name of the application. This is used by certain GPU providers
    /// to identify the currently running application and provide their specific optimisations.
    const char *appName;
    /// @brief The reference name of the application's engine. This is used by certain GPU
    /// providers to identify the currently utilised engine and provide their specific optimisations.
    const char *engineName;
    /// @brief A list of names of all enabled extensions. Names are determined as per the backend;
    /// for a list of all possible extensions, one should check with the utilised backend's docs itself.
    const char **enabledExtensions;
    /// @brief How many items in enabledExtensions. If enabledExtensions is NULL, this value must be 0.
    uint32_t enabledExtensionsCount;
    /// @brief A value representing the internal app version, used by certain GPU providers alongside
    /// the appName to select which of their version-specific optimisations to apply.
    uint32_t appVersion;
    /// @brief A value representing the internal engine version, used by certain GPU providers alongside
    /// the engineVersion to select which of their version-specific optimisations to apply.
    uint32_t engineVersion;
    /// @brief The preferred graphics API backend to use.
    LGFXBackendType backend;
    /// @brief Whether runtime GPU error checking should be enabled. For certain backends like Vulkan,
    /// this relies on the user having the associated SDK installed. Thus it is not recommended for
    /// release builds.
    bool runtimeErrorChecking;
    /// @brief If true, the application will not attempt to disable environment variables that
    /// have been identified as causing crashes in many applications. Thus, it is recommended that this
    /// value stay as false.
    bool allowProblematicEnvironmentVariables;
} LGFXInstanceCreateInfo;

typedef int32_t (*LGFXCreateWindowSurfaceFunc)(LGFXDevice, void *, void *, void **);
typedef struct LGFXSwapchainCreateInfo
{
    void *windowHandle;
    // void *nativeWindowHandle;
    // void *displayHandle;
    LGFXCreateWindowSurfaceFunc createSurfaceFunc;
    LGFXSwapchainPresentationMode presentationMode;
    uint32_t width;
    uint32_t height;
    LGFXSwapchain oldSwapchain;
} LGFXSwapchainCreateInfo;

typedef struct LGFXDeviceFeatures
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

typedef struct LGFXDeviceCreateInfo
{
    LGFXDeviceFeatures requiredFeatures;
    uint32_t maxDescriptorSets;
} LGFXDeviceCreateInfo;

typedef struct LGFXTextureCreateInfo
{
    LGFXTextureFormat format;
    LGFXTextureUsage usage;
    uint32_t mipLevels;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t sampleCount;

    void *externalTextureHandle;
    const char *memoryIdentifierName;
} LGFXTextureCreateInfo;

typedef struct LGFXSamplerStateCreateInfo
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
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t sampleCount;
    LGFXMemoryBlock textureMemory;
    LGFXDevice device;
} LGFXTextureImpl;

typedef struct LGFXRenderProgramImpl
{
    void *handle;
    LGFXDevice device;

    LGFXRenderTarget *targets;
    uint32_t targetsCount;

    LGFXRenderTarget currentTarget;

    LGFXRenderAttachmentInfo *attachments;
    uint32_t attachmentsCount;

    bool outputToBackbuffer;
} LGFXRenderProgramImpl;

typedef struct LGFXBufferCreateInfo
{
    size_t size;
    LGFXBufferUsage bufferUsage;
    LGFXMemoryUsage memoryUsage;

    const char *memoryIdentifierName;
} LGFXBufferCreateInfo;
typedef struct LGFXBufferImpl
{
    void *handle;
    LGFXBufferUsage usage;
    LGFXMemoryBlock bufferMemory;
    LGFXDevice device;
    size_t size;
} LGFXBufferImpl;

typedef struct LGFXRenderTargetCreateInfo
{
    LGFXTexture *textures;
    uint32_t texturesCount;
    LGFXRenderProgram forRenderProgram;
} LGFXRenderTargetCreateInfo;
typedef struct LGFXRenderTargetImpl
{
    void *handle;
    LGFXTexture *textures;
    uint32_t texturesCount;
    LGFXDevice device;
} LGFXRenderTargetImpl;

/// @brief An attachment refers to the state of the textures passing in and out of each renderpass
typedef struct LGFXRenderAttachmentInfo
{
    LGFXTextureFormat format;
    LGFXRenderAttachmentOutput outputType;
    uint32_t samples;
    bool clear;
} LGFXRenderAttachmentInfo;
/// @brief A pass is a stage of the render program. Currently, all passes in a program execute sequentially, and depend on the completion of the previous pass to execute.
typedef struct LGFXRenderPassInfo
{
    int32_t *colorAttachmentIDs;
    uint32_t colorAttachmentsCount;
    int32_t depthAttachmentID;
    /// @brief The index of the multi-sample-enabled render pass to resolve for
    int32_t resolveAttachmentID;

    int32_t *readAttachmentIDs;
    uint32_t readAttachmentsCount;
} LGFXRenderPassInfo;
typedef struct LGFXRenderProgramCreateInfo
{
    LGFXRenderAttachmentInfo *attachments;
    uint32_t attachmentsCount;

    LGFXRenderPassInfo *renderPasses;
    uint32_t renderPassCount;

    bool outputToBackbuffer;
    uint32_t maxBackbufferTexturesCount;
} LGFXRenderProgramCreateInfo;

typedef struct LGFXVertexAttribute
{
    LGFXVertexElementFormat format;
    uint32_t offset;
} LGFXVertexAttribute;
typedef struct LGFXVertexDeclaration
{
    LGFXVertexAttribute *elements;
    uint32_t elementsCount;
    bool isPerInstance;
    bool isTightlyPacked;
    uint32_t packedSize;
} LGFXVertexDeclaration;
LGFXVertexDeclaration LGFXCreateVertexDeclaration(LGFXVertexElementFormat *elementFormats, uint32_t elementsCount, bool isPerInstance, bool tightlyPacked);

typedef struct LGFXBlendState
{
    LGFXBlend sourceColorBlend;
    LGFXBlend sourceAlphaBlend;
    LGFXBlend destinationColorBlend;
    LGFXBlend destinationAlphaBlend;
} LGFXBlendState;

static inline bool LGFXBlendStateEquals(const LGFXBlendState left, LGFXBlendState right)
{
    return left.sourceAlphaBlend == right.sourceAlphaBlend && left.sourceColorBlend == right.sourceColorBlend && left.destinationAlphaBlend == right.destinationAlphaBlend && left.destinationColorBlend == right.destinationColorBlend;
}

#define DISABLE_BLEND {LGFXBlend_Disable, LGFXBlend_Disable, LGFXBlend_Disable, LGFXBlend_Disable}
#define ADDITIVE_BLEND {LGFXBlend_SourceAlpha, LGFXBlend_SourceAlpha, LGFXBlend_One, LGFXBlend_One}
#define ALPHA_BLEND {LGFXBlend_One, LGFXBlend_One, LGFXBlend_InverseSourceAlpha, LGFXBlend_One}
#define NON_PREMULTIPLIED_BLEND {LGFXBlend_SourceAlpha, LGFXBlend_SourceAlpha, LGFXBlend_InverseSourceAlpha, LGFXBlend_One}
#define OPAQUE_BLEND {LGFXBlend_One, LGFXBlend_One, LGFXBlend_Zero, LGFXBlend_Zero}

static inline LGFXBlendState LGFXBlendStateDisabled()
{
    LGFXBlendState result = DISABLE_BLEND;
    return result;
}
static inline LGFXBlendState LGFXBlendStateAdditive()
{
    LGFXBlendState result = ADDITIVE_BLEND;
    return result;
}
static inline LGFXBlendState LGFXBlendStateAlphaBlend()
{
    LGFXBlendState result = ALPHA_BLEND;
    return result;
}
static inline LGFXBlendState LGFXBlendStateNonPremultiplied()
{
    LGFXBlendState result = NON_PREMULTIPLIED_BLEND;
    return result;
}
static inline LGFXBlendState LGFXBlendStateOpaque()
{
    LGFXBlendState result = OPAQUE_BLEND;
    return result;
}

#ifndef LGFX_FENCE_POOL_SIZE
#define LGFX_FENCE_POOL_SIZE 8
#endif

typedef struct LGFXFunctionVariableMetadata
{
    LGFXShaderResourceType type;
    uint32_t set;
    uint32_t binding;
    uint32_t arrayLength;
    uint32_t inputAttachmentIndex;
    uint32_t size;
} LGFXFunctionVariableMetadata;

typedef struct LGFXFunctionVariable
{
    void **currentValues;
    uint32_t valuesCount;
    void *infos;
    LGFXFunctionVariableMetadata variableMetadata;

    LGFXDevice device;
    bool valueIsOwnedBuffer;
} LGFXFunctionVariable;

typedef struct LGFXFunctionCreateInfo
{
    LGFXFunctionType type;

    uint32_t *module1Data;
    size_t module1DataLength;

    uint32_t *module2Data;
    size_t module2DataLength;

    LGFXFunctionVariableBatchTemplate *variableBatchTemplates;
    uint32_t variableBatchTemplatesCount;

} LGFXFunctionCreateInfo;

typedef struct LGFXFunctionVariableBatchTemplateCreateInfo
{
    const LGFXFunctionVariableMetadata *variables;
    uint32_t variablesCount;
} LGFXFunctionVariableBatchTemplateCreateInfo;

typedef struct LGFXShaderPipelineCreateInfo
{
    LGFXFunction function;
    bool dynamicLineWidth;
    bool depthTest;
    bool depthWrite;

    LGFXVertexDeclaration *vertexDeclarations;
    uint32_t vertexDeclarationCount;

    LGFXPrimitiveType primitiveType;
    LGFXCullMode cullMode;
    LGFXBlendState blendState;

    LGFXRenderProgram forRenderProgram;
    uint32_t forRenderPass;

    const char *entryPoint1Name;
    const char *entryPoint2Name;
} LGFXShaderPipelineCreateInfo;

LGFX_EXPORT LGFXInstance LGFXCreateInstance(LGFXInstanceCreateInfo *info);
LGFX_EXPORT void LGFXDestroyInstance(LGFXInstance instance);
LGFX_EXPORT uint8_t LGFXGetPixelSize(LGFXTextureFormat format);

LGFX_EXPORT LGFXFence LGFXCreateFence(LGFXDevice device, bool signalled);
LGFX_EXPORT LGFXFence LGFXRentFence(LGFXDevice device, bool signalled);
LGFX_EXPORT void LGFXReturnRentedFence(LGFXDevice device, LGFXFence fence);
LGFX_EXPORT void LGFXAwaitFence(LGFXFence fence);
LGFX_EXPORT void LGFXResetFence(LGFXFence fence);
LGFX_EXPORT void LGFXDestroyFence(LGFXFence fence);

LGFX_EXPORT LGFXSemaphore LGFXCreateSemaphore(LGFXDevice device);
LGFX_EXPORT void LGFXDestroySemaphore(LGFXSemaphore semaphore);
LGFX_EXPORT LGFXSemaphore LGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain);
LGFX_EXPORT LGFXSemaphore LGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain);
LGFX_EXPORT uint32_t LGFXSwapchainGetCurrentFrameIndex(LGFXSwapchain swapchain);
LGFX_EXPORT uint32_t LGFXSwapchainGetCurrentImageIndex(LGFXSwapchain swapchain);
LGFX_EXPORT LGFXTexture LGFXSwapchainGetCurrentFrame(LGFXSwapchain swapchain);
LGFX_EXPORT LGFXTexture LGFXSwapchainGetCurrentImage(LGFXSwapchain swapchain);

LGFX_EXPORT void LGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType);
LGFX_EXPORT void LGFXAwaitDraw(LGFXCommandBuffer commandBuffer);
LGFX_EXPORT void LGFXAwaitGraphicsIdle(LGFXDevice device);

LGFX_EXPORT LGFXDevice LGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info);
LGFX_EXPORT void LGFXDestroyDevice(LGFXDevice device);

LGFX_EXPORT LGFXSwapchain LGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info);
LGFX_EXPORT uint32_t LGFXSwapchainGetBackbufferTexturesCount(LGFXSwapchain swapchain);
LGFX_EXPORT void LGFXAwaitSwapchainIdle(LGFXSwapchain swapchain);
LGFX_EXPORT void LGFXSwapchainInvalidate(LGFXSwapchain swapchain);
LGFX_EXPORT void LGFXSwapchainSetPresentationMode(LGFXSwapchain swapchain, LGFXSwapchainPresentationMode mode);
LGFX_EXPORT void LGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed);

LGFX_EXPORT LGFXTexture LGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info);
LGFX_EXPORT void LGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, uint32_t mipToTransition, uint32_t mipTransitionDepth);
LGFX_EXPORT void LGFXTextureSetData(LGFXDevice device, LGFXTexture texture, uint8_t *bytes, size_t length);
LGFX_EXPORT void LGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, uint32_t toMip);
LGFX_EXPORT void LGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, LGFXPoint3 extents, LGFXPoint3 offset, uint32_t toMip);
LGFX_EXPORT void LGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, uint32_t toMip);
LGFX_EXPORT void LGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, uint32_t fromMip, LGFXPoint3 toOffset, uint32_t toMip, LGFXPoint3 copyAreaSize, bool autoTransition);
LGFX_EXPORT void LGFXClearTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture texture, LGFXClearValues clearValues, uint32_t firstMipToTransition, uint32_t mipsToTransitionDepth, bool autoTransition);
LGFX_EXPORT void LGFXDestroyTexture(LGFXTexture texture);

LGFX_EXPORT LGFXSamplerState LGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info);
LGFX_EXPORT void LGFXDestroySamplerState(LGFXSamplerState state);

LGFX_EXPORT LGFXCommandBuffer LGFXCreateCommandBuffer(LGFXDevice device, bool forCompute);
LGFX_EXPORT void LGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission);
LGFX_EXPORT void LGFXCommandBufferEnd(LGFXCommandBuffer buffer);
LGFX_EXPORT void LGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore);
LGFX_EXPORT void LGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain);
LGFX_EXPORT void LGFXCommandBufferReset(LGFXCommandBuffer buffer);
LGFX_EXPORT void LGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer);

LGFX_EXPORT LGFXRenderTarget LGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info);
LGFX_EXPORT void LGFXDestroyRenderTarget(LGFXRenderTarget target);

LGFX_EXPORT LGFXBuffer LGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info);
LGFX_EXPORT void LGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to, size_t fromBufferOffset, size_t setIntoBufferOffset);
LGFX_EXPORT void LGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength);
LGFX_EXPORT void LGFXSetBufferDataFast(LGFXBuffer buffer, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength);
LGFX_EXPORT void LGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, uint32_t value);
LGFX_EXPORT void LGFXDestroyBuffer(LGFXBuffer buffer);
LGFX_EXPORT void *LGFXGetBufferData(LGFXBuffer buffer, size_t *bytesLength);
LGFX_EXPORT bool LGFXBufferResize(LGFXBuffer buffer, size_t newSize);
LGFX_EXPORT void *LGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(size_t));

LGFX_EXPORT LGFXRenderProgram LGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info);
LGFX_EXPORT void LGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures);
LGFX_EXPORT void LGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures);
LGFX_EXPORT void LGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer);
LGFX_EXPORT void LGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer);
LGFX_EXPORT void LGFXDestroyRenderProgram(LGFXRenderProgram program);

LGFX_EXPORT LGFXFunctionVariableBatchTemplate LGFXCreateFunctionVariableBatchTemplate(LGFXDevice device, const LGFXFunctionVariableBatchTemplateCreateInfo *info);
LGFX_EXPORT LGFXFunctionVariableBatch LGFXCreateFunctionVariableBatchFromTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate fromTemplate);
LGFX_EXPORT void LGFXDestroyFunctionVariableBatchTemplate(LGFXDevice device, LGFXFunctionVariableBatchTemplate toDestroy);

LGFX_EXPORT LGFXFunction LGFXCreateFunction(LGFXDevice device, const LGFXFunctionCreateInfo *info);
LGFX_EXPORT void LGFXDestroyFunction(LGFXFunction func);

LGFX_EXPORT LGFXFunctionVariable LGFXCreateFunctionVariableSlot(LGFXDevice device, LGFXFunctionVariableBatchTemplate batchTemplate, uint32_t forVariableOfIndex);
LGFX_EXPORT LGFXFunctionVariable LGFXCreateFunctionVariable(LGFXDevice device, LGFXFunctionVariableMetadata *info);
LGFX_EXPORT void LGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *functionVariables, uint32_t variablesCount);
LGFX_EXPORT void LGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction, uint32_t setIndex);
LGFX_EXPORT void LGFXDestroyFunctionVariable(LGFXFunctionVariable variable);

LGFX_EXPORT LGFXShaderPipeline LGFXCreateShaderPipeline(LGFXDevice device, LGFXShaderPipelineCreateInfo *info);
LGFX_EXPORT void LGFXDestroyShaderPipeline(LGFXShaderPipeline shaderPipeline);
LGFX_EXPORT void LGFXUseShaderPipeline(LGFXCommandBuffer buffer, LGFXShaderPipeline shaderPipeline);

LGFX_EXPORT void LGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area);
LGFX_EXPORT void LGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area);

LGFX_EXPORT void LGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, size_t offset);
LGFX_EXPORT void LGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, uint32_t vertexBuffersCount);
LGFX_EXPORT void LGFXDrawIndexed(LGFXCommandBuffer commands, uint32_t indexCount, uint32_t instances, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);
LGFX_EXPORT void LGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, size_t bufferOffset, size_t drawCount, size_t drawParamsStride);

LGFX_EXPORT void LGFXDispatchCompute(LGFXCommandBuffer commands, uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ);
LGFX_EXPORT void LGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, size_t offset);

LGFX_EXPORT bool LGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, uint32_t frameWidth, uint32_t frameHeight);
LGFX_EXPORT void LGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain);

#ifdef __cplusplus
}
#endif