#include "vulkan/LGFXVulkan.h"
#include "LGFXImpl.h"
#include "volk.h"
#include "Logging.h"
#include "memory.h"
#include "sync.h"
#include <string.h>
#include <math.h>
#include "vulkan/vk_mem_alloc.h"

#define ONE_OVER_255 0.00392156862f

// VULKAN-SPECIFIC HELPER STRUCTS
typedef struct LGFXMemoryBlockImpl
{
	VmaAllocation vkAllocation;
	VmaAllocationInfo vkAllocationInfo;
} LGFXMemoryBlockImpl;

LGFXMemoryBlock VkLGFXAllocMemoryForTexture(LGFXDevice device, LGFXTexture texture, LGFXMemoryUsage memoryUsage);
LGFXMemoryBlock VkLGFXAllocMemoryForBuffer(LGFXDevice device, LGFXBuffer buffer, LGFXMemoryUsage memoryUsage);
// END

// HELPER FUNCTIONS
inline VkBlendFactor LGFXBlendState2Vulkan(LGFXBlend blend)
{
    switch (blend)
    {
        case LGFXBlend_DestinationAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case LGFXBlend_DestinationColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case LGFXBlend_SourceAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case LGFXBlend_SourceColor:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case LGFXBlend_InverseDestinationAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case LGFXBlend_InverseDestinationColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case LGFXBlend_InverseSourceAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case LGFXBlend_InverseSourceColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        
        case LGFXBlend_One:
            return VK_BLEND_FACTOR_ONE;
        case LGFXBlend_Disable:
            return VK_BLEND_FACTOR_ZERO;

        default:
            return VK_BLEND_FACTOR_ONE;
    }
}

inline VkPrimitiveTopology LGFXPrimitiveType2Vulkan(LGFXPrimitiveType type)
{
	switch (type)
	{
		case LGFXPrimitiveType_TriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case LGFXPrimitiveType_TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case LGFXPrimitiveType_LineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case LGFXPrimitiveType_LineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case LGFXPrimitiveType_PointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case LGFXPrimitiveType_TriangleFan:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

inline VkFormat LGFXVertexElementFormat2Vulkan(LGFXVertexElementFormat format)
{
    switch (format)
    {
        case LGFXVertexElementFormat_Float:
            return VK_FORMAT_R32_SFLOAT;
        case LGFXVertexElementFormat_Vector2:
            return VK_FORMAT_R32G32_SFLOAT;
        case LGFXVertexElementFormat_Vector3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case LGFXVertexElementFormat_Vector4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
		case LGFXVertexElementFormat_Color:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case LGFXVertexElementFormat_Int:
            return VK_FORMAT_R32_SINT;
        case LGFXVertexElementFormat_Uint:
            return VK_FORMAT_R32_UINT;
		default:
			return VK_FORMAT_UNDEFINED;
	}
}

inline VkBufferUsageFlags LGFXBufferUsage2Vulkan(LGFXBufferUsage usage)
{
	return (VkBufferUsageFlags)usage;
}

inline VkPresentModeKHR LGFXSwapchainPresentationMode2Vulkan(LGFXSwapchainPresentationMode mode)
{
	switch (mode)
	{
		case LGFXSwapchainPresentationMode_Fifo:
			return VK_PRESENT_MODE_FIFO_KHR;
		case LGFXSwapchainPresentationMode_Immediate:
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case LGFXSwapchainPresentationMode_Mailbox:
			return VK_PRESENT_MODE_MAILBOX_KHR;
	}
}
inline VkSamplerAddressMode LGFXSamplerRepeatMode2Vulkan(LGFXSamplerRepeatMode repeatMode)
{
	switch (repeatMode)
	{
		case LGFXSamplerRepeatMode_Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case LGFXSamplerRepeatMode_ClampToEdgeColor:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case LGFXSamplerRepeatMode_ClampToStaticColor:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	}
}
inline VkFilter LGFXFilterType2Vulkan(LGFXFilterType type)
{
	switch (type)
	{
		case LGFXFilterType_Linear:
			return VK_FILTER_LINEAR;
		case LGFXFilterType_Point:
			return VK_FILTER_NEAREST;
		case LGFXFilterType_Cubic:
			return VK_FILTER_CUBIC_EXT;
	}
}
inline VkBorderColor LGFXSamplerBorderColor2Vulkan(LGFXSamplerBorderColor color)
{
	switch (color)
	{
		case LGFXSamplerBorderColor_OpaqueBlack:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		case LGFXSamplerBorderColor_OpaqueWhite:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			
		case LGFXSamplerBorderColor_OpaqueBlackInt:
			return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		case LGFXSamplerBorderColor_OpaqueWhiteInt:
			return VK_BORDER_COLOR_INT_OPAQUE_WHITE;

		case LGFXSamplerBorderColor_TransparentBlack:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		case LGFXSamplerBorderColor_TransparentBlackInt:
			return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	}
}

inline VkImageUsageFlags LGFXTextureUsage2Vulkan(LGFXTextureUsage usage)
{
	//these map 1:1
	return (VkImageUsageFlags)usage;
}
inline VkImageLayout LGFXTextureLayout2Vulkan(LGFXTextureLayout layout)
{
	switch (layout)
	{
		case LGFXTextureLayout_Undefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case LGFXTextureLayout_General:
			return VK_IMAGE_LAYOUT_GENERAL;
		case LGFXTextureLayout_ColorAttachmentOptimal:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case LGFXTextureLayout_DepthStencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case LGFXTextureLayout_DepthStencilReadOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		case LGFXTextureLayout_ShaderReadOptimal:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case LGFXTextureLayout_TransferSrcOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		case LGFXTextureLayout_TransferDstOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		case LGFXTextureLayout_Preinitialized:
			return VK_IMAGE_LAYOUT_PREINITIALIZED;
		case LGFXTextureLayout_PresentSource:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case LGFXTextureLayout_SharedPresentSrc:
			return VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
		case LGFXTextureLayout_FragmentDensityMap:
			return VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
		case LGFXTextureLayout_FragmentShadingRateAttachmentOptimal:
			return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
		default:
			return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}
inline VkShaderStageFlags LGFXShaderInputAccess2Vulkan(LGFXShaderInputAccessFlags flags)
{
	VkShaderStageFlags result = 0;
	if ((flags & LGFXShaderInputAccess_Vertex) != 0)
	{
		result |= VK_SHADER_STAGE_VERTEX_BIT;
	}
	if ((flags & LGFXShaderInputAccess_Fragment) != 0)
	{
		result |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if ((flags & LGFXShaderInputAccess_Compute) != 0)
	{
		result |= VK_SHADER_STAGE_COMPUTE_BIT;
	}
	return result;
}
inline VkDescriptorType LGFXShaderResourceType2Vulkan(LGFXShaderResourceType type)
{
	switch (type)
	{
		case LGFXShaderResourceType_Uniform:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case LGFXShaderResourceType_Sampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		case LGFXShaderResourceType_Texture:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case LGFXShaderResourceType_StructuredBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case LGFXShaderResourceType_InputAttachment:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case LGFXShaderResourceType_StorageTexture:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		default:
			LGFX_ERROR("Unknown LGFXShaderResourceType %u\n", type);
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}
}
VkFormat LGFXTextureFormat2Vulkan(LGFXTextureFormat format)
{
	switch (format)
	{
		case LGFXTextureFormat_Undefined:
			return VK_FORMAT_UNDEFINED;
		case LGFXTextureFormat_R8Unorm:
			return VK_FORMAT_R8_UNORM;
		case LGFXTextureFormat_R8Snorm:
			return VK_FORMAT_R8_SNORM;
		case LGFXTextureFormat_R8Uint:
			return VK_FORMAT_R8_UINT;
		case LGFXTextureFormat_R8Sint:
			return VK_FORMAT_R8_SINT;
		case LGFXTextureFormat_R16Uint:
			return VK_FORMAT_R16_UINT;
		case LGFXTextureFormat_R16Sint:
			return VK_FORMAT_R16_SINT;
		case LGFXTextureFormat_R16Float:
			return VK_FORMAT_R16_SFLOAT;
		case LGFXTextureFormat_RG8Unorm:
			return VK_FORMAT_R8G8_UNORM;
		case LGFXTextureFormat_RG8Snorm:
			return VK_FORMAT_R8G8_SNORM;
		case LGFXTextureFormat_RG8Uint:
			return VK_FORMAT_R8G8_UINT;
		case LGFXTextureFormat_RG8Sint:
			return VK_FORMAT_R8G8_SINT;
		case LGFXTextureFormat_R32Float:
			return VK_FORMAT_R32_SFLOAT;
		case LGFXTextureFormat_R32Uint:
			return VK_FORMAT_R32_UINT;
		case LGFXTextureFormat_R32Sint:
			return VK_FORMAT_R32_SINT;
		case LGFXTextureFormat_RG16Uint:
			return VK_FORMAT_R16G16_UINT;
		case LGFXTextureFormat_RG16Sint:
			return VK_FORMAT_R16G16_SINT;
		case LGFXTextureFormat_RG16Float:
			return VK_FORMAT_R16G16_SFLOAT;
		case LGFXTextureFormat_RGBA8Unorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case LGFXTextureFormat_RGBA8UnormSrgb:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case LGFXTextureFormat_RGBA8Snorm:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case LGFXTextureFormat_RGBA8Uint:
			return VK_FORMAT_R8G8B8A8_UINT;
		case LGFXTextureFormat_RGBA8Sint:
			return VK_FORMAT_R8G8B8A8_SINT;
		case LGFXTextureFormat_BGRA8Unorm:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case LGFXTextureFormat_BGRA8UnormSrgb:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case LGFXTextureFormat_RGB10A2Uint:
			return VK_FORMAT_A2R10G10B10_UINT_PACK32;
		case LGFXTextureFormat_RGB10A2Unorm:
			return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
		case LGFXTextureFormat_RG11B10Ufloat:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case LGFXTextureFormat_RGB9E5Ufloat:
			return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		case LGFXTextureFormat_RG32Float:
			return VK_FORMAT_R32G32_SFLOAT;
		case LGFXTextureFormat_RG32Uint:
			return VK_FORMAT_R32G32_UINT;
		case LGFXTextureFormat_RG32Sint:
			return VK_FORMAT_R32G32_SINT;
		case LGFXTextureFormat_RGBA16Uint:
			return VK_FORMAT_R16G16B16A16_UINT;
		case LGFXTextureFormat_RGBA16Sint:
			return VK_FORMAT_R16G16B16A16_SINT;
		case LGFXTextureFormat_RGBA16Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case LGFXTextureFormat_RGBA32Float:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case LGFXTextureFormat_RGBA32Uint:
			return VK_FORMAT_R32G32B32A32_UINT;
		case LGFXTextureFormat_RGBA32Sint:
			return VK_FORMAT_R32G32B32A32_SINT;
		case LGFXTextureFormat_Stencil8:
			return VK_FORMAT_S8_UINT;
		case LGFXTextureFormat_Depth16Unorm:
			return VK_FORMAT_D16_UNORM;
		case LGFXTextureFormat_Depth24Plus:
			return VK_FORMAT_X8_D24_UNORM_PACK32;
		case LGFXTextureFormat_Depth24PlusStencil8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case LGFXTextureFormat_Depth32Float:
			return VK_FORMAT_D32_SFLOAT;
		case LGFXTextureFormat_Depth32FloatStencil8:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case LGFXTextureFormat_BC1RGBAUnorm:
			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case LGFXTextureFormat_BC1RGBAUnormSrgb:
			return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
		case LGFXTextureFormat_BC2RGBAUnorm:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		case LGFXTextureFormat_BC2RGBAUnormSrgb:
			return VK_FORMAT_BC2_SRGB_BLOCK;
		case LGFXTextureFormat_BC3RGBAUnorm:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case LGFXTextureFormat_BC3RGBAUnormSrgb:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case LGFXTextureFormat_BC4RUnorm:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		case LGFXTextureFormat_BC4RSnorm:
			return VK_FORMAT_BC4_SNORM_BLOCK;
		case LGFXTextureFormat_BC5RGUnorm:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case LGFXTextureFormat_BC5RGSnorm:
			return VK_FORMAT_BC5_SNORM_BLOCK;
		case LGFXTextureFormat_BC6HRGBUfloat:
			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case LGFXTextureFormat_BC6HRGBFloat:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case LGFXTextureFormat_BC7RGBAUnorm:
			return VK_FORMAT_BC7_SRGB_BLOCK;
		case LGFXTextureFormat_BC7RGBAUnormSrgb:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		case LGFXTextureFormat_ETC2RGB8Unorm:
			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
		case LGFXTextureFormat_ETC2RGB8UnormSrgb:
			return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
		case LGFXTextureFormat_ETC2RGB8A1Unorm:
			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
		case LGFXTextureFormat_ETC2RGB8A1UnormSrgb:
			return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
		case LGFXTextureFormat_ETC2RGBA8Unorm:
			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		case LGFXTextureFormat_ETC2RGBA8UnormSrgb:
			return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
		case LGFXTextureFormat_EACR11Unorm:
			return VK_FORMAT_EAC_R11_UNORM_BLOCK;
		case LGFXTextureFormat_EACR11Snorm:
			return VK_FORMAT_EAC_R11_SNORM_BLOCK;
		case LGFXTextureFormat_EACRG11Unorm:
			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		case LGFXTextureFormat_EACRG11Snorm:
			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC4x4Unorm:
			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC4x4UnormSrgb:
			return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC5x4Unorm:
			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC5x4UnormSrgb:
			return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC5x5Unorm:
			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC5x5UnormSrgb:
			return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC6x5Unorm:
			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC6x5UnormSrgb:
			return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC6x6Unorm:
			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC6x6UnormSrgb:
			return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC8x5Unorm:
			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC8x5UnormSrgb:
			return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC8x6Unorm:
			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC8x6UnormSrgb:
			return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC8x8Unorm:
			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC8x8UnormSrgb:
			return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC10x5Unorm:
			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC10x5UnormSrgb:
			return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC10x6Unorm:
			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC10x6UnormSrgb:
			return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC10x8Unorm:
			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC10x8UnormSrgb:
			return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC10x10Unorm:
			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC10x10UnormSrgb:
			return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC12x10Unorm:
			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC12x10UnormSrgb:
			return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
		case LGFXTextureFormat_ASTC12x12Unorm:
			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
		case LGFXTextureFormat_ASTC12x12UnormSrgb:
			return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;

		default:
			LGFX_ERROR("Unknown LGFXTextureFormat %u\n", format);
			return VK_FORMAT_UNDEFINED;
	}
}

typedef struct VkLGFXSwapchainSupport
{
    VkSurfaceCapabilitiesKHR capabilities;

	u32 supportedSurfaceFormatsCount;
	VkSurfaceFormatKHR * supportedSurfaceFormats;

	u32 supportedPresentModesCount;
	VkPresentModeKHR * presentModes;
} VkLGFXSwapchainSupport;

VkSurfaceFormatKHR VkLGFXFindSurface(VkColorSpaceKHR colorSpace, VkFormat format, VkLGFXSwapchainSupport *supported)
{
    for (u32 i = 0; i < supported->supportedSurfaceFormatsCount; i++)
    {
        if (supported->supportedSurfaceFormats[i].colorSpace == colorSpace && supported->supportedSurfaceFormats[i].format == format)
        {
            return supported->supportedSurfaceFormats[i];
        }
    }
	return supported->supportedSurfaceFormats[0];
}

LGFXCommandBuffer VkLGFXCreateTemporaryCommandBuffer(LGFXDevice device, LGFXCommandQueue queueToUse, bool alsoBeginBuffer)
{
	LGFXCommandBuffer result = Allocate(LGFXCommandBufferImpl, 1);
	result->queue = queueToUse;

	VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = queueToUse->transientCommandPool;
    allocInfo.pNext = NULL;

	EnterLock(queueToUse->commandPoolLock);

    if (vkAllocateCommandBuffers((VkDevice)device->logicalDevice, &allocInfo, (VkCommandBuffer *)&result->cmdBuffer) != VK_SUCCESS)
    {
        result = NULL;
    }

	ExitLock(queueToUse->commandPoolLock);

	if (alsoBeginBuffer && result != NULL)
    {
        VkCommandBufferBeginInfo beginInfo = {0};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = NULL;
        beginInfo.pNext = NULL;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		result->begun = true;

		vkBeginCommandBuffer((VkCommandBuffer)result->cmdBuffer, &beginInfo);
	}
    return result;
}
void VkLGFXEndTemporaryCommandBuffer(LGFXDevice device, LGFXCommandBuffer buffer)
{
    vkEndCommandBuffer((VkCommandBuffer)buffer->cmdBuffer);

	VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(VkCommandBuffer)buffer->cmdBuffer;

	//LGFXFence tempFence = VkLGFXCreateFence(device, false);

	//submit the queue
    EnterLock(buffer->queue->queueLock);

    if (vkQueueSubmit((VkQueue)buffer->queue->queue, 1, &submitInfo, NULL) != VK_SUCCESS)
    {
		LGFX_ERROR("Failed to submit queue\n");
    }

	ExitLock(buffer->queue->queueLock);

	vkQueueWaitIdle((VkQueue)buffer->queue->queue);
	//todo: see if can transfer this to a fence or something
    //vkWaitForFences(gpu->logicalDevice, 1, &queueToUse->queueFence, true, UINT64_MAX);
    //vkResetFences(gpu->logicalDevice, 1, &queueToUse->queueFence);
	//LGFXAwaitFence();

	//finally, free the command buffer
	EnterLock(buffer->queue->commandPoolLock);

	//LGFXDestroyFence(tempFence);
	vkFreeCommandBuffers((VkDevice)device->logicalDevice, (VkCommandPool)buffer->queue->transientCommandPool, 1, (VkCommandBuffer *)&buffer->cmdBuffer);
	//vkFreeCommandBuffers(gpu->logicalDevice, queueToUse->transientCommandPool, 1, &commandBuffer);

	ExitLock(buffer->queue->commandPoolLock);
	free(buffer);
}

LGFXFence VkLGFXCreateFence(LGFXDevice device, bool signalled)
{
	VkFenceCreateInfo fenceCreateInfo = {0};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = NULL;
	fenceCreateInfo.flags = signalled ? VK_FENCE_CREATE_SIGNALED_BIT : NULL;

	VkFence fence;
	if (vkCreateFence((VkDevice)device->logicalDevice, &fenceCreateInfo, NULL, &fence) != VK_SUCCESS)
	{
		return NULL;
	}
	LGFXFenceImpl *result = Allocate(LGFXFenceImpl, 1);
	result->fence = fence;
	result->device = device;

	return result;
}
void VkLGFXAwaitFence(LGFXFence fence)
{
	vkWaitForFences((VkDevice)fence->device->logicalDevice, 1, (VkFence *)&fence->fence, VK_TRUE, UINT64_MAX);
}
void VkLGFXResetFence(LGFXFence fence)
{
	vkResetFences((VkDevice)fence->device->logicalDevice, 1, (VkFence *)&fence->fence);
}

LGFXSemaphore VkLGFXCreateSemaphore(LGFXDevice device)
{
	VkSemaphoreCreateInfo semCreateInfo = {0};
	semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkSemaphore semaphore;
	if (vkCreateSemaphore((VkDevice)device->logicalDevice, &semCreateInfo, NULL, &semaphore) != VK_SUCCESS)
	{
		return NULL;
	}
	LGFXSemaphore result = Allocate(LGFXSemaphoreImpl, 1);
	result->semaphore = semaphore;
	result->device = device;

	return result;
}

void VkLGFXAwaitDraw(LGFXCommandBuffer commandBuffer)
{
	VkMemoryBarrier2 memoryBarrier = {0};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	memoryBarrier.srcAccessMask = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
	memoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_2_NONE;
	memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
	memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

	VkDependencyInfo dependency = {0};
	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.memoryBarrierCount = 1;
	dependency.pMemoryBarriers = &memoryBarrier;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	vkCmdPipelineBarrier2((VkCommandBuffer)commandBuffer->cmdBuffer, &dependency);
}
void VkLGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType)
{
	VkMemoryBarrier2 memoryBarrier = {0};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	
	if (funcType == LGFXFunctionType_Compute)
	{
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
	}
	else if (funcType == LGFXFunctionType_Fragment || funcType == LGFXFunctionType_Vertex)
	{
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		memoryBarrier.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT;
	}
	else
	{
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		memoryBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if ((opType & LGFXFunctionOperationType_IndexBufferRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_INDEX_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_IndirectBufferRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_VertexBufferRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_UniformBufferRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_UNIFORM_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_ComputeBufferRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_ComputeTextureRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
	}
	if ((opType & LGFXFunctionOperationType_FragmentFunctionRead) != 0)
	{
		memoryBarrier.dstStageMask |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		memoryBarrier.dstAccessMask |= VK_ACCESS_2_SHADER_READ_BIT;
	}

	VkDependencyInfo dependency = {0};
	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.memoryBarrierCount = 1;
	dependency.pMemoryBarriers = &memoryBarrier;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	vkCmdPipelineBarrier2((VkCommandBuffer)commandBuffer->cmdBuffer, &dependency);
}
void VkLGFXAwaitGraphicsIdle(LGFXDevice device)
{
	vkQueueWaitIdle((VkQueue)device->graphicsQueue->queue);
}

LGFXCommandQueue VkLGFXCreateCommandQueue(LGFXDevice device, u32 queueFamilyID, VkQueue vkQueue)
{
	LGFXCommandQueueImpl *result = Allocate(LGFXCommandQueueImpl, 1);

	result->queue = vkQueue;
	result->queueFamilyID = queueFamilyID;
	result->queueLock = NewLock();
	result->fence = VkLGFXCreateFence(device, true);

	result->inDevice = device;

	result->commandPoolLock = NewLock();

	VkCommandPoolCreateInfo commandPoolCreateInfo = {0};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	commandPoolCreateInfo.queueFamilyIndex = result->queueFamilyID;
	commandPoolCreateInfo.pNext = NULL;

	if (vkCreateCommandPool((VkDevice)device->logicalDevice, &commandPoolCreateInfo, NULL, (VkCommandPool *)&result->transientCommandPool) != VK_SUCCESS)
	{
		LGFX_ERROR("Error creating transient command pool for queue family %u\n", queueFamilyID);
	}

	VkCommandPoolCreateInfo cmdPoolCreateInfo = {0};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = queueFamilyID; // AstralCanvasVk_GetCurrentGPU()->queueInfo.dedicatedGraphicsQueueIndex;
	if (vkCreateCommandPool((VkDevice)device->logicalDevice, &cmdPoolCreateInfo, NULL, (VkCommandPool *)&result->regularCommandPool) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create regular command pool for queue family %u\n", queueFamilyID);
	}

	return result;
}

void VkLGFXGetQueueCreateInfos(VkLGFXQueueProperties *properties, VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo *outInfos, u32 *outInfoCount, float *nilPriorityTodo)
{
	VkQueueFamilyProperties *all = (VkQueueFamilyProperties *)properties->allQueueInfos;

	i32 maxGraphicsScore = -100;
	i32 maxComputeScore = -100;
	i32 maxTransferScore = -100;
	for (i32 i = 0; i < (i32)properties->allQueueInfosCount; i++)
    {
        VkQueueFamilyProperties property = all[i];
        i32 graphicsScore = 0;
		i32 computeScore = 0;
		i32 transferScore = 0;
		//if we dont have a transfer bit, dont bother with this queue
        if ((property.queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
        {
            graphicsScore--;
			computeScore--;
			transferScore++;
		}
        if ((property.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
			graphicsScore++;
			computeScore--;
			transferScore--;
		}
        if ((property.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
        {
			graphicsScore--;
			computeScore++;
			transferScore--;
        }

        if (graphicsScore > maxGraphicsScore)
        {
            maxGraphicsScore = graphicsScore;
            properties->dedicatedGraphicsQueueIndex = i;
        }
        if (computeScore > maxComputeScore)
        {
            maxComputeScore = computeScore;
            properties->dedicatedComputeQueueIndex = i;
        }
        if (transferScore > maxTransferScore)
        {
            maxTransferScore = transferScore;
            properties->dedicatedTransferQueueIndex = i;
        }
    }

	VkDeviceQueueCreateInfo *info = &outInfos[*outInfoCount];
	info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	info->queueCount = 1; // todo: 300000 queues from each family shall be sacrificed
	info->queueFamilyIndex = (u32)properties->dedicatedGraphicsQueueIndex;
	info->pQueuePriorities = nilPriorityTodo;
	info->pNext = NULL;
	info->flags = NULL;
	*outInfoCount += 1;

	if (properties->dedicatedComputeQueueIndex != properties->dedicatedGraphicsQueueIndex)
	{
		info = &outInfos[*outInfoCount];
		info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info->queueCount = 1;
		info->queueFamilyIndex = (u32)properties->dedicatedComputeQueueIndex;
		info->pQueuePriorities = nilPriorityTodo;
		info->pNext = NULL;
		info->flags = NULL;
		*outInfoCount += 1;
	}
	if (properties->dedicatedTransferQueueIndex != properties->dedicatedGraphicsQueueIndex)
	{
		info = &outInfos[*outInfoCount];
		info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info->queueCount = 1;
		info->queueFamilyIndex = (u32)properties->dedicatedTransferQueueIndex;
		info->pQueuePriorities = nilPriorityTodo;
		info->pNext = NULL;
		info->flags = NULL;
		*outInfoCount += 1;
	}
}

u32 VkLGFXDefaultIsValidDevice(VkPhysicalDevice device, LGFXDeviceFeatures requiredFeatures)
{
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	if (
		(requiredFeatures.multiDrawIndirect						&& !features.multiDrawIndirect) ||
		(requiredFeatures.depthClamp							&& !features.depthClamp) ||
		(requiredFeatures.fillModeNonSolid						&& !features.fillModeNonSolid) ||
		(requiredFeatures.wideLines								&& !features.wideLines) ||
		(requiredFeatures.textureCompressionETC2				&& !features.textureCompressionETC2) ||
		(requiredFeatures.textureCompressionASTC				&& !features.textureCompressionASTC_LDR) ||
		(requiredFeatures.textureCompressionBC					&& !features.textureCompressionBC) ||
		(requiredFeatures.vertexShaderCanStoreDataAndAtomics	&& !features.vertexPipelineStoresAndAtomics) ||
		(requiredFeatures.fragmentShaderCanStoreDataAndAtomics	&& !features.fragmentStoresAndAtomics) ||
		(requiredFeatures.shaderFloat64							&& !features.shaderFloat64) ||
		(requiredFeatures.shaderInt64							&& !features.shaderInt64) ||
		(requiredFeatures.shaderInt16							&& !features.shaderInt16) ||
		(requiredFeatures.sparseBinding							&& !features.sparseBinding) ||
		(requiredFeatures.bindlessUniformBufferArrays			&& !features.shaderUniformBufferArrayDynamicIndexing) ||
		(requiredFeatures.bindlessSamplerAndTextureArrays		&& !features.shaderSampledImageArrayDynamicIndexing) ||
		(requiredFeatures.bindlessStorageBufferArrays			&& !features.shaderStorageBufferArrayDynamicIndexing) ||
		(requiredFeatures.bindlessStorageTextureArrays			&& !features.shaderStorageImageArrayDynamicIndexing)
	)
	{
		return 0;
	}

	u32 totalScore = 1;
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		totalScore += 2;
	}
	else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
	{
		totalScore += 1;
	}

	return totalScore;
}
VkLGFXSwapchainSupport VkLGFXQuerySwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface)
{
	VkLGFXSwapchainSupport details = {0};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &details.supportedSurfaceFormatsCount, NULL);

	details.supportedSurfaceFormats = Allocate(VkSurfaceFormatKHR, details.supportedSurfaceFormatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &details.supportedSurfaceFormatsCount, details.supportedSurfaceFormats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &details.supportedPresentModesCount, NULL);

	details.presentModes = Allocate(VkPresentModeKHR, details.supportedPresentModesCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &details.supportedPresentModesCount, details.presentModes);

    return details;
}
// END

// MAIN IMPLEMENTATIONS
VkBool32 VkLGFXErrorFunc(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if (messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LGFX_ERROR("ERROR: %s\n", pCallbackData->pMessage);
	}
	else
	{
		LGFX_WARN("WARNING: %s\n", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

LGFXInstance VkLGFXCreateInstance(LGFXInstanceCreateInfo *info)
{
	LGFXInstanceImpl *result = Allocate(LGFXInstanceImpl, 1);
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {0};

	result->runtimeErrorChecking = info->runtimeErrorChecking;
	result->backend = info->backend;

	//load vulkan functions
    VkResult volkInitResult = volkInitialize();

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = info->appName;
	appInfo.pEngineName = info->engineName;
	appInfo.apiVersion = LGFX_VULKAN_API_VERSION;
	appInfo.engineVersion = info->engineVersion;
	appInfo.applicationVersion = info->appVersion;
	appInfo.pNext = NULL;


	VkInstanceCreateInfo instanceInfo = {0};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	if (info->runtimeErrorChecking)
	{
		result->enabledInstanceExtensions = Allocate(const char *, info->enabledExtensionsCount + 1);
		for (u32 i = 0; i < info->enabledExtensionsCount; i++)
		{
			result->enabledInstanceExtensions[i] = info->enabledExtensions[i];
		}
		result->enabledInstanceExtensions[info->enabledExtensionsCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

		instanceInfo.enabledExtensionCount = info->enabledExtensionsCount + 1;
		instanceInfo.ppEnabledExtensionNames = result->enabledInstanceExtensions;
	}
	else
	{
		result->enabledInstanceExtensions = Allocate(const char *, info->enabledExtensionsCount);
		for (u32 i = 0; i < info->enabledExtensionsCount; i++)
		{
			result->enabledInstanceExtensions[i] = info->enabledExtensions[i];
		}
		
		instanceInfo.enabledExtensionCount = info->enabledExtensionsCount;
		instanceInfo.ppEnabledExtensionNames = result->enabledInstanceExtensions;
	}
	instanceInfo.flags = 0;

	if (info->runtimeErrorChecking)
	{
		text errorChecker = "VK_LAYER_KHRONOS_validation";
		result->enabledErrorCheckerExtensions = Allocate(const char *, 1);
		result->enabledErrorCheckerExtensions[0] = errorChecker;
		//check validation layer support
		u32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);

		VkLayerProperties *layerProperties = Allocate(VkLayerProperties, layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties);

		u32 totalSupported = 0;
		for (usize i = 0; i < layerCount; i++)
		{
			//printf("instance has layer %s\n", layerProperties[i].layerName);
			if (strcmp(errorChecker, layerProperties[i].layerName) == 0)
			{
				totalSupported++;
			}
		}
		if (totalSupported != 1)
		{
			LGFX_ERROR("Not all requested error checkers are supported\n");
			VkLGFXDestroyInstance(result);
			return NULL;
		}

		instanceInfo.enabledLayerCount = 1;
		instanceInfo.ppEnabledLayerNames = result->enabledErrorCheckerExtensions;

		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;//StructureType.DebugUtilsMessengerCreateInfoExt;
		debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugMessengerCreateInfo.pNext = NULL;
		debugMessengerCreateInfo.pUserData = NULL;
		debugMessengerCreateInfo.pfnUserCallback = &VkLGFXErrorFunc;
		instanceInfo.pNext = &debugMessengerCreateInfo;

		free(layerProperties);
	}
	else
	{
		result->enabledErrorCheckerExtensions = NULL;
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = NULL;
		instanceInfo.pNext = NULL;
	}

	result->instance = Allocate(VkInstance, 1);
	if (vkCreateInstance(&instanceInfo, NULL, (VkInstance *)&result->instance) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create instance\n");
		VkLGFXDestroyInstance(result);
		return NULL;
	}

	volkLoadInstanceOnly((VkInstance)result->instance);

	return result;
}
LGFXDevice VkLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info)
{
	VkInstance vkInstance = (VkInstance)instance->instance;
	u32 deviceCount = 0;
	if (vkEnumeratePhysicalDevices(vkInstance, &deviceCount, NULL) != VK_SUCCESS || deviceCount == 0)
	{
		//No GPUs with Vulkan support
		return NULL;
	}

	VkPhysicalDevice *physDevices = Allocate(VkPhysicalDevice, deviceCount);
	const char *extensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; //, VK_KHR_SURFACE_EXTENSION_NAME };

	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, physDevices);

	u32 topScore = 0;
	VkPhysicalDevice bestPhysicalDevice = NULL;
	for (usize i = 0; i < deviceCount; i++)
	{
		u32 currentScore = VkLGFXDefaultIsValidDevice(physDevices[i], info->requiredFeatures);
		if (currentScore > topScore)
		{
			topScore = currentScore;
			bestPhysicalDevice = physDevices[i];
		}
	}
	free(physDevices);

	//no device can work
	if (bestPhysicalDevice == NULL)
	{
		LGFX_ERROR("Failed to locate a useable physical device\n");
		return NULL;
	}

	//maximum 3 queue families to be created from
	//some GPUs go up to 5 queue families or more, but we don't need that at this point
	u32 propertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(bestPhysicalDevice, &propertiesCount, NULL);

	VkQueueFamilyProperties *allQueueFamilyProperties = Allocate(VkQueueFamilyProperties, propertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(bestPhysicalDevice, &propertiesCount, allQueueFamilyProperties);


	//hopefully get the triumvirate of queues: Graphics, Transfer and Compute
	//otherwise, the graphics queue (guaranteed to exist by this point) will serve as fallback
	VkLGFXQueueProperties inputQueueProps;
	inputQueueProps.allQueueInfos = allQueueFamilyProperties;
	inputQueueProps.allQueueInfosCount = propertiesCount;
	inputQueueProps.dedicatedComputeQueueIndex = -1;
	inputQueueProps.dedicatedGraphicsQueueIndex = -1;
	inputQueueProps.dedicatedTransferQueueIndex = -1;

	VkDeviceQueueCreateInfo queueCreateInfos[3];
	u32 finalQueueCreateInfoCount = 0;
	float priority = 0.0f;
	VkLGFXGetQueueCreateInfos(&inputQueueProps, bestPhysicalDevice, queueCreateInfos, &finalQueueCreateInfoCount, &priority);

	VkPhysicalDeviceSynchronization2Features sync2 = {0};
	sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	sync2.pNext = NULL;
	sync2.synchronization2 = VK_TRUE;

	VkPhysicalDeviceFeatures2 deviceEnabledFeatures = {0};
	deviceEnabledFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceEnabledFeatures.pNext = &sync2;
	//no easier way to do this... oh well
	deviceEnabledFeatures.features.fillModeNonSolid = true;
	deviceEnabledFeatures.features.multiDrawIndirect = info->requiredFeatures.multiDrawIndirect;
	deviceEnabledFeatures.features.depthClamp = info->requiredFeatures.depthClamp;
	deviceEnabledFeatures.features.fillModeNonSolid = info->requiredFeatures.fillModeNonSolid;
	deviceEnabledFeatures.features.wideLines = info->requiredFeatures.wideLines;
	deviceEnabledFeatures.features.textureCompressionETC2 = info->requiredFeatures.textureCompressionETC2;
	deviceEnabledFeatures.features.textureCompressionASTC_LDR = info->requiredFeatures.textureCompressionASTC;
	deviceEnabledFeatures.features.textureCompressionBC = info->requiredFeatures.textureCompressionBC;
	deviceEnabledFeatures.features.vertexPipelineStoresAndAtomics = info->requiredFeatures.vertexShaderCanStoreDataAndAtomics;
	deviceEnabledFeatures.features.fragmentStoresAndAtomics = info->requiredFeatures.fragmentShaderCanStoreDataAndAtomics;
	deviceEnabledFeatures.features.shaderFloat64 = info->requiredFeatures.shaderFloat64;
	deviceEnabledFeatures.features.shaderInt64 = info->requiredFeatures.shaderInt64;
	deviceEnabledFeatures.features.shaderInt16 = info->requiredFeatures.shaderInt16;
	deviceEnabledFeatures.features.sparseBinding = info->requiredFeatures.sparseBinding;
	deviceEnabledFeatures.features.shaderUniformBufferArrayDynamicIndexing = info->requiredFeatures.bindlessUniformBufferArrays;
	deviceEnabledFeatures.features.shaderSampledImageArrayDynamicIndexing = info->requiredFeatures.bindlessSamplerAndTextureArrays;
	deviceEnabledFeatures.features.shaderStorageBufferArrayDynamicIndexing = info->requiredFeatures.bindlessStorageBufferArrays;
	deviceEnabledFeatures.features.shaderStorageImageArrayDynamicIndexing = info->requiredFeatures.bindlessStorageTextureArrays;

	VkDeviceCreateInfo logicalDeviceInfo = {0};
	logicalDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceInfo.queueCreateInfoCount = finalQueueCreateInfoCount;
	logicalDeviceInfo.pQueueCreateInfos = queueCreateInfos;
	logicalDeviceInfo.enabledExtensionCount = 1; //require swapchain support only
	logicalDeviceInfo.ppEnabledExtensionNames = extensionNames;
	logicalDeviceInfo.enabledLayerCount = 0; //these basically dont exist
	logicalDeviceInfo.ppEnabledLayerNames = NULL;
	logicalDeviceInfo.pNext = &deviceEnabledFeatures;
	//logicalDeviceInfo.pEnabledFeatures = &deviceEnabledFeatures;

	VkDevice logicalDevice;
	VkResult vkResult = vkCreateDevice(bestPhysicalDevice, &logicalDeviceInfo, NULL, &logicalDevice);
	if (vkResult != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create device\n");
		return NULL;
	}

	volkLoadDevice(logicalDevice);

	LGFXDevice result = Allocate(LGFXDeviceImpl, 1);
	result->instance = instance;
	result->physicalDevice = bestPhysicalDevice;
	result->logicalDevice = logicalDevice;
	result->backend = instance->backend;

	{
		VkQueue graphicsQueue;
		vkGetDeviceQueue(logicalDevice, inputQueueProps.dedicatedGraphicsQueueIndex, 0, &graphicsQueue);

		result->graphicsQueue = VkLGFXCreateCommandQueue(result, inputQueueProps.dedicatedGraphicsQueueIndex, graphicsQueue);
	}

	if (inputQueueProps.dedicatedComputeQueueIndex != inputQueueProps.dedicatedGraphicsQueueIndex)
	{
		VkQueue computeQueue;
		vkGetDeviceQueue(logicalDevice, inputQueueProps.dedicatedComputeQueueIndex, 0, &computeQueue);

		result->computeQueue = VkLGFXCreateCommandQueue(result, inputQueueProps.dedicatedComputeQueueIndex, computeQueue);
	}
	else
	{
		result->computeQueue = result->graphicsQueue;
	}

	if (inputQueueProps.dedicatedTransferQueueIndex != inputQueueProps.dedicatedGraphicsQueueIndex)
	{
		VkQueue transferQueue;
		vkGetDeviceQueue(logicalDevice, inputQueueProps.dedicatedTransferQueueIndex, 0, &transferQueue);

		result->transferQueue = VkLGFXCreateCommandQueue(result, inputQueueProps.dedicatedTransferQueueIndex, transferQueue);
	}
	else
	{
		result->transferQueue = result->graphicsQueue;
	}

	VmaVulkanFunctions vulkanAllocatorFunctions = {0};
	vulkanAllocatorFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vulkanAllocatorFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCreateInfo = {0};
	allocatorCreateInfo.device = logicalDevice;
	allocatorCreateInfo.instance = (VkInstance)instance->instance;
	allocatorCreateInfo.physicalDevice = bestPhysicalDevice;
	allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
	allocatorCreateInfo.pAllocationCallbacks = NULL;
	allocatorCreateInfo.pDeviceMemoryCallbacks = NULL;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.pVulkanFunctions = &vulkanAllocatorFunctions;
	if (vmaCreateAllocator(&allocatorCreateInfo, (VmaAllocator *)&result->memoryAllocator) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create memory allocator\n");
		return NULL;
	}

	u32 maxUniformDescriptors = info->maxDescriptorSets == 0 ? 1024 : info->maxDescriptorSets;
	VkDescriptorPoolSize poolSizes[6];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSizes[0].descriptorCount = maxUniformDescriptors;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[1].descriptorCount = maxUniformDescriptors;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[2].descriptorCount = maxUniformDescriptors;
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[3].descriptorCount = maxUniformDescriptors;
	poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[4].descriptorCount = maxUniformDescriptors;
	poolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizes[5].descriptorCount = maxUniformDescriptors;

	VkDescriptorPoolCreateInfo poolCreateInfo = {0};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.poolSizeCount = 6;
	poolCreateInfo.maxSets = maxUniformDescriptors;

	VkDescriptorPool mainPool;
	if (vkCreateDescriptorPool(logicalDevice, &poolCreateInfo, NULL, &mainPool) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create shader uniform descriptor pool\n");
		return NULL;
	}

	result->descriptorPool = mainPool;

	return result;
}

LGFXSwapchain VkLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info)
{
	VkSurfaceKHR surfaceKHR;

	if (info->oldSwapchain == NULL)
	{
#if defined(WINDOWS)
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {0};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = info->nativeWindowHandle;
	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);

	vkCreateWin32SurfaceKHR((VkInstance)device->instance->instance, &surfaceCreateInfo, NULL, &surfaceKHR);
#elif defined(LINUX)
	VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = {0};
	#error TODO
#elif defined(MACOS)
	VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = {0};
	#error TODO
#endif
	}
	else
	{
		surfaceKHR = (VkSurfaceKHR)info->oldSwapchain->windowSurface;
	}

	VkLGFXSwapchainSupport details = VkLGFXQuerySwapchainSupportDetails((VkPhysicalDevice)device->physicalDevice, surfaceKHR);
	VkSurfaceFormatKHR surfaceFormat = VkLGFXFindSurface(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_FORMAT_B8G8R8A8_UNORM, &details);

	LGFXSwapchain result = Allocate(LGFXSwapchainImpl, 1);
	result->presentMode = info->presentationMode;
	result->swapchain = NULL;
	result->currentImageIndex = 0;
	result->width = info->width;
	result->height = info->height;
	result->nativeWindowHandle = info->nativeWindowHandle;
	result->windowSurface = surfaceKHR;
	result->device = device;
	result->justCreated = true;
	result->invalidated = false;

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surfaceKHR;
	createInfo.minImageCount = details.capabilities.minImageCount;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;

    createInfo.preTransform = details.capabilities.currentTransform; // VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = result->presentMode;
    createInfo.clipped = true;
	if (info->oldSwapchain != NULL)
	{
		createInfo.oldSwapchain = info->oldSwapchain->swapchain;
	}
	createInfo.imageExtent.width = result->width;
	createInfo.imageExtent.height = result->height;

    if (vkCreateSwapchainKHR((VkDevice)device->logicalDevice, &createInfo, NULL, (VkSwapchainKHR *)&result->swapchain) != VK_SUCCESS)
    {
		LGFX_ERROR("Error creating swapchain\n");
		return NULL;
    }

	if (vkGetSwapchainImagesKHR((VkDevice)device->logicalDevice, (VkSwapchainKHR)result->swapchain, &result->backbufferTexturesCount, NULL)!= VK_SUCCESS)
    {
		LGFX_ERROR("Error creating swapchain\n");
		return NULL;
    }
	result->backbufferTextures = Allocate(LGFXTexture, result->backbufferTexturesCount);
	result->backDepthbuffers = Allocate(LGFXTexture, result->backbufferTexturesCount);
	VkImage *backbufferImageHandles = Allocate(VkImage, result->backbufferTexturesCount);
	vkGetSwapchainImagesKHR((VkDevice)device->logicalDevice, (VkSwapchainKHR)result->swapchain, &result->backbufferTexturesCount, backbufferImageHandles);

	for (u32 i = 0; i < result->backbufferTexturesCount; i++)
	{
		LGFXTextureCreateInfo textureCreateInfo = {0};

		textureCreateInfo.externalTextureHandle = backbufferImageHandles[i];
		textureCreateInfo.format = LGFXTextureFormat_BGRA8Unorm;
		textureCreateInfo.width = result->width;
		textureCreateInfo.height = result->height;
		textureCreateInfo.depth = 1;
		textureCreateInfo.mipLevels = 1;
		textureCreateInfo.sampleCount = 1;
		textureCreateInfo.usage = LGFXTextureUsage_ColorAttachment;
		result->backbufferTextures[i] = LGFXCreateTexture(device, &textureCreateInfo);

		textureCreateInfo.depth = 1;
		textureCreateInfo.externalTextureHandle = NULL;
		textureCreateInfo.width = result->width;
		textureCreateInfo.height = result->height;
		textureCreateInfo.format = LGFXTextureFormat_Depth32Float;
		textureCreateInfo.mipLevels = 1;
		textureCreateInfo.sampleCount = 1;
		textureCreateInfo.usage = (LGFXTextureUsage)(LGFXTextureUsage_DepthAttachment | LGFXTextureUsage_Sampled);
		result->backDepthbuffers[i] = LGFXCreateTexture(device, &textureCreateInfo);
		assert(result->backDepthbuffers[i]->imageView != NULL);
	}

	if (info->oldSwapchain != NULL)
	{
		//take ownership of old swapchain synchronization primitives
		result->fence = info->oldSwapchain->fence;
		result->awaitPresentComplete = info->oldSwapchain->awaitPresentComplete;
		result->awaitRenderComplete = info->oldSwapchain->awaitRenderComplete;

		info->oldSwapchain->fence = NULL;
		info->oldSwapchain->awaitPresentComplete = NULL;
		info->oldSwapchain->awaitRenderComplete = NULL;
	}
	else
	{
		result->fence = LGFXCreateFence(device, true);
		result->awaitPresentComplete = LGFXCreateSemaphore(device);
		result->awaitRenderComplete = LGFXCreateSemaphore(device);
	}

	free(backbufferImageHandles);

	return result;
}
bool VkLGFXSwapchainSwapBuffers(LGFXSwapchain *swapchain, u32 currentBackbufferWidth, u32 currentBackbufferHeight)
{
	LGFXSwapchain currentSwapchain = *swapchain;
	VkResult result = VK_ERROR_OUT_OF_DATE_KHR;
	if (currentSwapchain != NULL)
	{
		if (!currentSwapchain->invalidated)
		{
			LGFXAwaitFence(currentSwapchain->fence);
			result = vkAcquireNextImageKHR((VkDevice)currentSwapchain->device->logicalDevice, (VkSwapchainKHR)currentSwapchain->swapchain, UINT64_MAX, (VkSemaphore)currentSwapchain->awaitPresentComplete->semaphore, NULL, &currentSwapchain->currentImageIndex);
		}
		else
		{
			printf("Invalid swapchain detected, recreating\n");
		}
	}
    if (result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR)// && !currentSwapchain->recreatedAtFrameEnd)
        {
			LGFXSwapchainCreateInfo createInfo = {0};
			createInfo.width = currentBackbufferWidth;
			createInfo.height = currentBackbufferHeight;
			createInfo.oldSwapchain = currentSwapchain;
			createInfo.presentationMode = currentSwapchain->presentMode;
			//to check: do we need to re-acquire this every time we need to change?
			createInfo.nativeWindowHandle = currentSwapchain->nativeWindowHandle;

			*swapchain = LGFXCreateSwapchain(currentSwapchain->device, &createInfo);
			printf("Swapchain recreated successfully\n");
			LGFXDestroySwapchain(currentSwapchain, false);
			printf("Old swapchain disposed\n");

			//resized
			return true;
        }
        else if (result != VK_SUBOPTIMAL_KHR)
        {
			LGFX_ERROR("Failed to acquire next swapchain image, error code %u\n", result);
			return false;
        }
    }
	LGFXResetFence(currentSwapchain->fence);
    return false;
}

bool VkLGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, u32 frameWidth, u32 frameHeight)
{
	if (VkLGFXSwapchainSwapBuffers(swapchain, frameWidth, frameHeight))
	{
		//ask the drawing loop to yield until next frame
		return false;
	}

	return true;
}
void VkLGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain)
{
	VkSemaphore awaitRender = (VkSemaphore)swapchain->awaitRenderComplete->semaphore;

	VkPresentInfoKHR presentInfo = {0};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = (VkSwapchainKHR *)&swapchain->swapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &awaitRender;
	presentInfo.pImageIndices = &swapchain->currentImageIndex;

	EnterLock(device->graphicsQueue->queueLock);
	VkResult presentResults = vkQueuePresentKHR((VkQueue)device->graphicsQueue->queue, &presentInfo);
	//swapchain->renderTargets.data[swapchain->currentImageIndex].textures.data[0]->imageLayout = (u32)VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	ExitLock(device->graphicsQueue->queueLock);

	if (swapchain->justCreated)
	{
		swapchain->justCreated = false;
	}

	if (presentResults == VK_ERROR_OUT_OF_DATE_KHR || presentResults == VK_SUBOPTIMAL_KHR)
	{
		printf("End-of-frame invalidation detected, will attempt to recreate swapchain next frame\n");
		swapchain->invalidated = true;

		presentResults = VK_SUCCESS;
		//swapChain.Recreate(SwapChain.QuerySwapChainSupport(CurrentGPU.Device));
	}

	if (presentResults != VK_SUCCESS)
	{
		LGFX_ERROR("Error presenting queue");
	}
}

LGFXTexture VkLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info)
{
	VkImage image;

	VkImageType imageType;
	if (info->width == 0 || info->height == 0)
	{
		imageType = VK_IMAGE_TYPE_1D;
	}
	else if (info->depth <= 1)
	{
		imageType = VK_IMAGE_TYPE_2D;
	}
	else
	{
		imageType = VK_IMAGE_TYPE_3D;
	}

	if (info->externalTextureHandle == NULL)
	{
		VkImageCreateInfo createInfo = {0};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

		createInfo.extent.width = info->width;
		createInfo.extent.height = info->height;
		createInfo.extent.depth = info->depth == 0 ? 1 : info->depth;
		createInfo.format = LGFXTextureFormat2Vulkan(info->format);
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage = LGFXTextureUsage2Vulkan(info->usage);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.samples = (VkSampleCountFlags)info->sampleCount;
		createInfo.imageType = imageType;
		createInfo.arrayLayers = 1;
		createInfo.mipLevels = 1;
		createInfo.flags = 0;

		if (vkCreateImage((VkDevice)device->logicalDevice, &createInfo, NULL, &image) != VK_SUCCESS)
		{
			LGFX_ERROR("Failed to create image\n");
		}
	}
	else
	{
		image = (VkImage)info->externalTextureHandle;
	}

	LGFXTextureImpl result;
	result.imageHandle = image;
	result.format = info->format;
	result.width = info->width;
	result.height = info->height;
	result.depth = info->depth;
	result.mipLevels = info->mipLevels;
	result.device = device;
	result.sampleCount = info->sampleCount;
	result.ownsHandle = info->externalTextureHandle == NULL;
	result.layout = LGFXTextureLayout_Undefined;

	if (result.ownsHandle)
	{
		result.textureMemory = VkLGFXAllocMemoryForTexture(device, &result, LGFXMemoryUsage_GPU_ONLY);
	}

	VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
	if (result.format == LGFXTextureFormat_Depth16Unorm || result.format == LGFXTextureFormat_Depth32Float)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (result.format > LGFXTextureFormat_Depth16Unorm)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageViewCreateInfo viewCreateInfo = {0};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = image;
	viewCreateInfo.format = LGFXTextureFormat2Vulkan(info->format);
	viewCreateInfo.viewType = imageType;

	viewCreateInfo.subresourceRange.aspectMask = imageAspect;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = result.mipLevels;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;

	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	if (vkCreateImageView((VkDevice)device->logicalDevice, &viewCreateInfo, NULL, (VkImageView*)&result.imageView) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create image view\n");
	}

	LGFXTexture ptr = Allocate(LGFXTextureImpl, 1);
	*ptr = result;
	return ptr;
}
//administer texture HRT (Free of charge)
void VkLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, u32 mipToTransition, u32 mipTransitionDepth)
{
	if (texture->layout == targetLayout)
	{
		return;
		//LGFX_ERROR("Attempting to transition texture layout to itself, layout: %u\n", targetLayout);
	}
	VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
	if (texture->format == LGFXTextureFormat_Depth16Unorm || texture->format == LGFXTextureFormat_Depth32Float)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (texture->format > LGFXTextureFormat_Depth16Unorm)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

    VkImageMemoryBarrier2 memBarrier = {0};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    memBarrier.oldLayout = LGFXTextureLayout2Vulkan(texture->layout);
    memBarrier.newLayout = LGFXTextureLayout2Vulkan(targetLayout);
    memBarrier.srcQueueFamilyIndex = 0;
    memBarrier.dstQueueFamilyIndex = 0;
    memBarrier.image = texture->imageHandle;
    memBarrier.subresourceRange.aspectMask = imageAspect;
    memBarrier.subresourceRange.baseMipLevel = mipToTransition;
    memBarrier.subresourceRange.levelCount = mipTransitionDepth == 0 ? texture->mipLevels : mipTransitionDepth;
    memBarrier.subresourceRange.baseArrayLayer = 0;
    memBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags2 sourceStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags2 destinationStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

    LGFXCommandQueue cmdQueue = device->graphicsQueue;
	LGFXCommandBuffer cmdBuffer = commandBuffer;
	if (commandBuffer == NULL)
    {
        cmdBuffer = VkLGFXCreateTemporaryCommandBuffer(device, cmdQueue, true);
    }

    if (cmdBuffer != NULL)
    {
        VkPipelineStageFlags depthStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        VkPipelineStageFlags sampledStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        switch (memBarrier.oldLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                sourceStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                sourceStage = depthStageMask;
                memBarrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                sourceStage = depthStageMask | sampledStageMask;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                sourceStage = sampledStageMask;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                sourceStage = VK_PIPELINE_STAGE_2_HOST_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				sourceStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				memBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
				break;

            default:
				LGFX_ERROR("Invalid source layout: %u\n", (u32)memBarrier.oldLayout);
                break;
        }

		switch (memBarrier.newLayout)
		{
			case VK_IMAGE_LAYOUT_GENERAL:
				destinationStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
				memBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				memBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				destinationStage = depthStageMask;
				memBarrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				destinationStage = depthStageMask | sampledStageMask;
				memBarrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
				memBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				memBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				memBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				// vkQueuePresentKHR performs automatic visibility operations
				break;

			default:
				LGFX_ERROR("Invalid destination layout: %u\n", (u32)memBarrier.oldLayout);
				break;
		}

		memBarrier.srcStageMask = sourceStage;
		memBarrier.dstStageMask = destinationStage;

		VkDependencyInfo dependencyInfo = {0};
		dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &memBarrier;
		dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		EnterLock(cmdQueue->commandPoolLock);
		vkCmdPipelineBarrier2((VkCommandBuffer)cmdBuffer->cmdBuffer, &dependencyInfo);
		ExitLock(cmdQueue->commandPoolLock);

		texture->layout = targetLayout;

		//only end and submit the buffer if the user did not provide one.
        //If the user did, then it's their role to end and submit
        if (commandBuffer == NULL)
        {
            VkLGFXEndTemporaryCommandBuffer(device, cmdBuffer);
        }
    }
}
LGFXMemoryBlock VkLGFXAllocMemoryForTexture(LGFXDevice device, LGFXTexture texture, LGFXMemoryUsage memoryUsage)
{
	VmaAllocator vma = (VmaAllocator)device->memoryAllocator;

	VmaAllocationCreateInfo allocationCreateInfo = {0};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
    if (memoryUsage == LGFXMemoryUsage_GPU_ONLY)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (memoryUsage == LGFXMemoryUsage_CPU_TO_GPU)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		allocationCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (memoryUsage == LGFXMemoryUsage_GPU_TO_CPU)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		allocationCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	}

    LGFXMemoryBlockImpl memoryAllocated;

    if (vmaAllocateMemoryForImage(vma, (VkImage)texture->imageHandle, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        LGFX_ERROR("Failed to create memory for image");
    }

    vmaBindImageMemory(vma, memoryAllocated.vkAllocation, (VkImage)texture->imageHandle);

	char chars[100];
	i32 len = sprintf(chars, "Texture: %u, %u", texture->width, texture->height);
	chars[len] = '\0';

	vmaSetAllocationName(vma, memoryAllocated.vkAllocation, chars);

	LGFXMemoryBlock result = Allocate(LGFXMemoryBlockImpl, 1);
	*result = memoryAllocated;
	return result;
}
void VkLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, u8* bytes, usize length)
{
	LGFXBufferCreateInfo stagingBufferInfo = {0};
	stagingBufferInfo.bufferUsage = LGFXBufferUsage_TransferSource;
	stagingBufferInfo.memoryUsage = LGFXMemoryUsage_CPU_TO_GPU;
	stagingBufferInfo.size = length;

	LGFXBuffer stagingBuffer = VkLGFXCreateBuffer(device, &stagingBufferInfo);

	memcpy(stagingBuffer->bufferMemory->vkAllocationInfo.pMappedData, bytes, length);

	LGFXCommandBuffer cmdBuffer = VkLGFXCreateTemporaryCommandBuffer(device, device->graphicsQueue, true);

	VkLGFXTextureTransitionLayout(device, texture, LGFXTextureLayout_TransferDstOptimal, cmdBuffer, 0, 1);

	VkLGFXCopyBufferToTexture(device, cmdBuffer, stagingBuffer, texture, 0);

	VkLGFXTextureTransitionLayout(device, texture, LGFXTextureLayout_ShaderReadOptimal, cmdBuffer, 0, 1);

	VkLGFXEndTemporaryCommandBuffer(device, cmdBuffer);

	VkLGFXDestroyBuffer(stagingBuffer);
}
void VkLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, u32 toMip)
{
	LGFXCommandBuffer transientCmdBuffer = commandBuffer;
	if (commandBuffer == NULL)
	{
		transientCmdBuffer = VkLGFXCreateTemporaryCommandBuffer(device, device->transferQueue, true);
	}

	VkBufferImageCopy bufferImageCopy = {0};
    
    bufferImageCopy.bufferOffset = 0;
    //only set values other than 0 if the image buffer is not tightly packed
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;

	VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
	if (to->format == LGFXTextureFormat_Depth16Unorm || to->format == LGFXTextureFormat_Depth32Float)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (to->format > LGFXTextureFormat_Depth16Unorm)
	{
		imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

    bufferImageCopy.imageSubresource.aspectMask = imageAspect;
    bufferImageCopy.imageSubresource.mipLevel = toMip;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;

	bufferImageCopy.imageOffset.x = 0;
	bufferImageCopy.imageOffset.y = 0;
	bufferImageCopy.imageOffset.z = 0;
    bufferImageCopy.imageExtent.width = to->width;
    bufferImageCopy.imageExtent.height = to->height;
    bufferImageCopy.imageExtent.depth = to->depth;

    vkCmdCopyBufferToImage((VkCommandBuffer)transientCmdBuffer->cmdBuffer, (VkBuffer)from->handle, (VkImage)to->imageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

	if (commandBuffer == NULL)
	{
		VkLGFXEndTemporaryCommandBuffer(device, transientCmdBuffer);
	}
}
void VkLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, u32 toMip)
{
	LGFXCommandBuffer transientCmdBuffer = commandBuffer;
	if (commandBuffer == NULL)
	{
		EnterLock(device->graphicsQueue->commandPoolLock);
		transientCmdBuffer = VkLGFXCreateTemporaryCommandBuffer(device, device->graphicsQueue, true);
	}

	LGFXTextureLayout originalLayout = from->layout;
	LGFXTextureTransitionLayout(device, from, LGFXTextureLayout_TransferSrcOptimal, transientCmdBuffer, 0, 1);

    VkBufferImageCopy bufferImageCopy = {0};
    
    bufferImageCopy.bufferOffset = 0;
    //only set values other than 0 if the image buffer is not tightly packed
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;

    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel = toMip;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;

	bufferImageCopy.imageOffset.x = 0;
	bufferImageCopy.imageOffset.y = 0;
	bufferImageCopy.imageOffset.z = 0;
	bufferImageCopy.imageExtent.width = from->width;
    bufferImageCopy.imageExtent.height = from->height;
    bufferImageCopy.imageExtent.depth = from->depth;

    vkCmdCopyImageToBuffer((VkCommandBuffer)transientCmdBuffer->cmdBuffer, (VkImage)from->imageHandle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, (VkBuffer)to->handle, 1, &bufferImageCopy);

	LGFXTextureTransitionLayout(device, from, originalLayout, transientCmdBuffer, 0, 1);

	if (commandBuffer == NULL)
	{
		VkLGFXEndTemporaryCommandBuffer(device, transientCmdBuffer);
		ExitLock(device->graphicsQueue->commandPoolLock);
	}
    //AstralCanvasVk_EndTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, transientCmdBuffer);
}
void VkLGFXDestroyTexture(LGFXTexture texture)
{
	vkDestroyImageView((VkDevice)texture->device->logicalDevice, (VkImageView)texture->imageView, NULL);
	if (texture->ownsHandle)
	{
		vmaFreeMemory((VmaAllocator)texture->device->memoryAllocator, texture->textureMemory->vkAllocation);
		vkDestroyImage((VkDevice)texture->device->logicalDevice, (VkImage)texture->imageHandle, NULL);
		//vkDestroyImage(gpu->logicalDevice, (VkImage)this->imageHandle, NULL);
		free(texture->textureMemory);
	}
	free(texture);
}

// void VkLGFXTextureBlit(LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXFilterType filter)
// {
// 	VkImageBlit regions;
// 	regions.dstSubresource.
// 	vkCmdBlitImage(
// 		(VkCommandBuffer)commandBuffer->cmdBuffer, 
// 		(VkImage)from->imageHandle, 
// 		LGFXTextureLayout2Vulkan(from->layout), 
// 		(VkImage)to->imageHandle,
// 		LGFXTextureLayout2Vulkan(to->layout),
// 		1,
// 		regions,
// 		LGFXFilterType2Vulkan(filter))
// }

LGFXRenderTarget VkLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info)
{
	LGFXRenderTarget target = Allocate(LGFXRenderTargetImpl, 1);

	VkImageView *views = Allocate(VkImageView, info->texturesCount);
	for (u32 i = 0; i < info->texturesCount; i++)
	{
		views[i] = (VkImageView)info->textures[i]->imageView;
	}

	VkFramebufferCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.width = info->textures[0]->width;
	createInfo.height = info->textures[0]->height;
	createInfo.layers = 1;
	createInfo.attachmentCount = info->texturesCount;
	createInfo.pAttachments = views;
	createInfo.flags = NULL;
	createInfo.renderPass = (VkRenderPass)info->forRenderProgram->handle;

	VkFramebuffer frameBuffer;
	if (vkCreateFramebuffer((VkDevice)device->logicalDevice, &createInfo, NULL, &frameBuffer) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create framebuffer\n");
	}

	free(views);

	target->texturesCount = info->texturesCount;
	target->textures = Allocate(LGFXTexture, info->texturesCount);
	for (u32 i = 0; i < info->texturesCount; i++)
	{
		target->textures[i] = info->textures[i];
	}
	target->handle = frameBuffer;
	target->device = device;

	return target;
}

LGFXBuffer VkLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info)
{
	VkBufferUsageFlags usageFlags = LGFXBufferUsage2Vulkan(info->bufferUsage);
	LGFXBufferImpl result;

    VkBufferCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = info->size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (info->size == 0)
	{
		LGFX_ERROR("Attempted to create empty buffer!");
	}
    if (vkCreateBuffer((VkDevice)device->logicalDevice, &createInfo, NULL, (VkBuffer *)&result.handle) != VK_SUCCESS)
    {
		LGFX_ERROR("Failed to create buffer\n");
	}

	result.usage = info->bufferUsage;
	result.bufferMemory = VkLGFXAllocMemoryForBuffer(device, &result, info->memoryUsage);
	result.device = device;
	result.size = info->size;

	LGFXBuffer ptr = Allocate(LGFXBufferImpl, 1);
	*ptr = result;
	return ptr;
}
void VkLGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to)
{
	VkBufferCopy regions;
	regions.size = from->size;
	regions.srcOffset = 0;
	regions.dstOffset = 0;
	vkCmdCopyBuffer((VkCommandBuffer)commandBuffer->cmdBuffer, (VkBuffer)from->handle, (VkBuffer)to->handle, 1, &regions);
}
void VkLGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, u8 *data, usize dataLength)
{
	LGFXBufferCreateInfo stagingBufferInfo = {0};
	stagingBufferInfo.bufferUsage = LGFXBufferUsage_TransferSource;
	stagingBufferInfo.memoryUsage = LGFXMemoryUsage_CPU_TO_GPU;
	stagingBufferInfo.size = dataLength;

	LGFXBuffer stagingBuffer = VkLGFXCreateBuffer(buffer->device, &stagingBufferInfo);

	memcpy(stagingBuffer->bufferMemory->vkAllocationInfo.pMappedData, data, dataLength);

	LGFXCommandBuffer cmdBuffer = commandBufferToUse;
	if (commandBufferToUse == NULL)
	{
		cmdBuffer = VkLGFXCreateTemporaryCommandBuffer(buffer->device, buffer->device->transferQueue, true);
	}

	VkLGFXCopyBufferToBuffer(buffer->device, cmdBuffer, stagingBuffer, buffer);

	if (commandBufferToUse == NULL)
	{
		VkLGFXEndTemporaryCommandBuffer(buffer->device, cmdBuffer);
	}

	VkLGFXDestroyBuffer(stagingBuffer);
}
void VkLGFXSetBufferDataFast(LGFXBuffer buffer, u8 *data, usize dataLength)
{
	memcpy(buffer->bufferMemory->vkAllocationInfo.pMappedData, data, dataLength);
}
void VkLGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, u32 value)
{
	vkCmdFillBuffer(cmdBuffer, (VkBuffer)buffer->handle, 0, buffer->size, value);
}
void *VkLGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(usize))
{
	if (allocateFunction == NULL)
	{
		allocateFunction = &malloc;
	}

	void *result = allocateFunction(buffer->size);

	LGFXBufferCreateInfo info;
	info.bufferUsage = LGFXBufferUsage_TransferDest;
	info.memoryUsage = LGFXMemoryUsage_GPU_TO_CPU;
	info.size = buffer->size;
	LGFXBuffer destBuffer = LGFXCreateBuffer(buffer->device, &info);

	LGFXCommandBuffer cmds = VkLGFXCreateTemporaryCommandBuffer(buffer->device, buffer->device->transferQueue, true);

	VkLGFXCopyBufferToBuffer(buffer->device, cmds, buffer, destBuffer);

	VkLGFXEndTemporaryCommandBuffer(buffer->device, cmds);

	memcpy(result, destBuffer->bufferMemory->vkAllocationInfo.pMappedData, buffer->size);

	LGFXDestroyBuffer(destBuffer);

	return result;
}
void *VkLGFXGetBufferData(LGFXBuffer buffer)
{
	return buffer->bufferMemory->vkAllocationInfo.pMappedData;
}
LGFXMemoryBlock VkLGFXAllocMemoryForBuffer(LGFXDevice device, LGFXBuffer buffer, LGFXMemoryUsage memoryUsage)
{
    VmaAllocator vma = (VmaAllocator)device->memoryAllocator;
    
    VmaAllocationCreateInfo allocationCreateInfo = {0};
    allocationCreateInfo.usage = memoryUsage;
    //if (createMapped)
    {
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    if (memoryUsage == LGFXMemoryUsage_GPU_ONLY)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (memoryUsage == LGFXMemoryUsage_CPU_TO_GPU)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		allocationCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (memoryUsage == LGFXMemoryUsage_GPU_TO_CPU)
	{
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		allocationCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	}

    LGFXMemoryBlockImpl memoryAllocated;

    if (vmaAllocateMemoryForBuffer(vma, (VkBuffer)buffer->handle, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        LGFX_ERROR("Failed to create memory for buffer\n");
    }

    vmaBindBufferMemory((VmaAllocator)device->memoryAllocator, memoryAllocated.vkAllocation, (VkBuffer)buffer->handle);
	vmaSetAllocationName((VmaAllocator)device->memoryAllocator, memoryAllocated.vkAllocation, "buffer");

	LGFXMemoryBlock result = Allocate(LGFXMemoryBlockImpl, 1);
	*result = memoryAllocated;
	return result;
}

LGFXRenderProgram VkLGFXCreateRenderProgram(LGFXDevice device, LGFXRenderProgramCreateInfo *info)
{
	LGFXRenderProgramImpl program;
	program.device = device;
	program.currentPass = 0;
	program.handle = NULL;
	program.outputToBackbuffer = info->outputToBackbuffer;
	program.attachmentsCount = info->attachmentsCount;
	program.attachments = Allocate(LGFXRenderAttachmentInfo, info->attachmentsCount);
	for (u32 i = 0; i < info->attachmentsCount; i++)
	{
		program.attachments[i] = info->attachments[i];
	}

	VkAttachmentDescription *attachmentInfos = Allocate(VkAttachmentDescription, info->attachmentsCount);
	for (u32 i = 0; i < info->attachmentsCount; i++)
	{
		VkAttachmentDescription attachmentInfo = {0};
		attachmentInfo.format = LGFXTextureFormat2Vulkan(info->attachments[i].format);
		attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		if (info->attachments[i].format < LGFXTextureFormat_Depth16Unorm)
		{
			attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			if (info->attachments[i].readByRenderTarget)
			{
				attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else
			{
				//not read = present lol
				attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}

			if (info->attachments[i].clear)
			{
				attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			}
			else
			{
				attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			}
		}
		else
		{
			attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			if (info->attachments[i].readByRenderTarget)
			{
				attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}
			else 
			{
				attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			if (info->attachments[i].clear)
			{
				attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				if (info->attachments[i].format == LGFXTextureFormat_Depth24PlusStencil8 || info->attachments[i].format == LGFXTextureFormat_Depth32FloatStencil8)
				{
					attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				}
			}
			else
			{
				attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				if (info->attachments[i].format == LGFXTextureFormat_Depth24PlusStencil8 || info->attachments[i].format == LGFXTextureFormat_Depth32FloatStencil8)
				{
					attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				}
			}
		}

		attachmentInfos[i] = attachmentInfo;
	}

	VkSubpassDescription *passInfos = Allocate(VkSubpassDescription, info->renderPassCount);
	for (u32 i = 0; i < info->renderPassCount; i++)
	{
		VkSubpassDescription passInfo = {0};

		if (info->renderPasses[i].colorAttachmentsCount > 0)
		{
			passInfo.colorAttachmentCount = info->renderPasses[i].colorAttachmentsCount;
			VkAttachmentReference *outputAttachments = Allocate(VkAttachmentReference, info->renderPasses[i].colorAttachmentsCount);

			for (u32 j = 0; j < info->renderPasses[i].colorAttachmentsCount; j++)
			{
				u32 attachmentID = info->renderPasses[i].colorAttachmentIDs[j];
				outputAttachments[j].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // LGFXTextureFormat2Vulkan(info->attachments[attachmentID].format);
				outputAttachments[j].attachment = attachmentID;
			}

			passInfo.pColorAttachments = outputAttachments;
		}
		else
		{
			passInfo.pColorAttachments = NULL;
		}

		if (info->renderPasses[i].readAttachmentsCount > 0)
		{
			passInfo.inputAttachmentCount = info->renderPasses[i].readAttachmentsCount;
			VkAttachmentReference *readAttachments = Allocate(VkAttachmentReference, info->renderPasses[i].readAttachmentsCount);

			for (u32 j = 0; j < info->renderPasses[i].readAttachmentsCount; j++)
			{
				u32 attachmentID = info->renderPasses[i].readAttachmentIDs[j];
				readAttachments[j].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //todo?
				readAttachments[j].attachment = attachmentID;
			}
			passInfo.pInputAttachments = readAttachments;
		}
		else
		{
			passInfo.pInputAttachments = NULL;
		}

		if (info->renderPasses[i].depthAttachmentID != -1)
		{
			VkAttachmentReference *depthAttachment = Allocate(VkAttachmentReference, 1);
			depthAttachment->attachment = (u32)info->renderPasses[i].depthAttachmentID;
			depthAttachment->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			passInfo.pDepthStencilAttachment = depthAttachment;
		}
		else
		{
			passInfo.pDepthStencilAttachment = NULL;
		}
		passInfo.pPreserveAttachments = NULL;
		passInfo.preserveAttachmentCount = 0;
		passInfo.pResolveAttachments = NULL;

		passInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		passInfos[i] = passInfo;
	}

	VkRenderPassCreateInfo renderPassInfo = {0};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.subpassCount = info->renderPassCount;
	renderPassInfo.pSubpasses = passInfos;
	renderPassInfo.pAttachments = attachmentInfos;
	renderPassInfo.attachmentCount = info->attachmentsCount;

	if (info->renderPassCount > 1)
	{
		VkSubpassDependency *subpassDependencies = Allocate(VkSubpassDependency, info->renderPassCount - 1);
		for (u32 i = 0; i < info->renderPassCount; i++)
		{
			subpassDependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			subpassDependencies[i].srcSubpass = (u32)i;
			if (i == info->renderPassCount - 1)
			{
				subpassDependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
			}
			else subpassDependencies[i].dstSubpass = (u32)(i + 1);
			subpassDependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			subpassDependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}

		renderPassInfo.dependencyCount = info->renderPassCount - 1;
		renderPassInfo.pDependencies = subpassDependencies;
	}
	else
	{
		// VkSubpassDependency *subpassDependencies = Allocate(VkSubpassDependency, 1);

		// subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		// subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// subpassDependencies[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		// subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		// subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		// subpassDependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		// subpassDependencies[0].dstSubpass = 0;

		renderPassInfo.dependencyCount = 0;
		//1;
		renderPassInfo.pDependencies = NULL;
		//subpassDependencies;
	}

	if (vkCreateRenderPass((VkDevice)device->logicalDevice, &renderPassInfo, NULL, (VkRenderPass *)&program.handle) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create render pass\n");
	}
	if (renderPassInfo.pSubpasses != NULL)
	{
		for (u32 i = 0; i < renderPassInfo.subpassCount; i++)
		{
			if (renderPassInfo.pSubpasses[i].pColorAttachments != NULL)
			{
				free(renderPassInfo.pSubpasses[i].pColorAttachments);
			}
			if (renderPassInfo.pSubpasses[i].pDepthStencilAttachment != NULL)
			{
				free(renderPassInfo.pSubpasses[i].pDepthStencilAttachment);
			}
			if (renderPassInfo.pSubpasses[i].pInputAttachments != NULL)
			{
				free(renderPassInfo.pSubpasses[i].pInputAttachments);
			}
		}
		free(renderPassInfo.pSubpasses);
	}
	if (renderPassInfo.pAttachments != NULL)
	{
		free(renderPassInfo.pAttachments);
	}
	if (renderPassInfo.pDependencies != NULL)
	{
		free(renderPassInfo.pDependencies);
	}

	if (info->outputToBackbuffer)
	{
		program.targets = Allocate(LGFXRenderTarget, 3);
		program.targets[0] = NULL;
		program.targets[1] = NULL;
		program.targets[2] = NULL;
		program.targetsCount = 3;
	}
	else
	{
		program.targets = Allocate(LGFXRenderTarget, 1);
		program.targets[0] = NULL;
		program.targetsCount = 1;
	}
	program.currentTarget = NULL;

	LGFXRenderProgram result = Allocate(LGFXRenderProgramImpl, 1);
	*result = program;
	return result;
}
void VkLGFXBeginRenderProgramSwapchain(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXSwapchain outputSwapchain, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
	u32 index = outputSwapchain->currentImageIndex;
	assert(index < program->targetsCount);
	if (program->targets[index] != NULL && outputSwapchain->justCreated)
	{
		for (u32 i = 0; i < 3; i++)
		{
			if (program->targets[i] != NULL)
			{
				LGFXDestroyRenderTarget(program->targets[i]);
				program->targets[i] = NULL;
			}
		}
	}
	if (program->targets[index] == NULL)
	{
		LGFXRenderTargetCreateInfo createInfo;
		createInfo.forRenderProgram = program;
		createInfo.textures = Allocate(LGFXTexture, program->attachmentsCount);
		createInfo.texturesCount = program->attachmentsCount;
		for (u32 i = 0; i < program->attachmentsCount; i++)
		{
			if (program->attachments[i].format < LGFXTextureFormat_Depth16Unorm)
			{
				if (!program->attachments[i].readByRenderTarget)
				{
					createInfo.textures[i] = outputSwapchain->backbufferTextures[index];
				}
				else
				{
					LGFX_ERROR("NOT IMPLEMENTED");
				}
			}
			else
			{
				createInfo.textures[i] = outputSwapchain->backDepthbuffers[index]; // LGFXCreateTexture(program->device, &textureInfo);
			}
		}
		program->targets[index] = LGFXCreateRenderTarget(program->device, &createInfo);

		free(createInfo.textures);
	}
	VkLGFXBeginRenderProgram(program, commandBuffer, program->targets[index], clearColor, autoTransitionTargetTextures);
}
void VkLGFXBeginRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer, LGFXRenderTarget outputTarget, LGFXColor clearColor, bool autoTransitionTargetTextures)
{
	program->currentTarget = outputTarget;
	if (autoTransitionTargetTextures)
	{
		//LGFXCommandBuffer tempBuffer = VkLGFXCreateTemporaryCommandBuffer(program->device, program->device->graphicsQueue, true);
		for (u32 i = 0; i < outputTarget->texturesCount; i++)
		{
			LGFXTexture *textures = outputTarget->textures;
			VkImageLayout newLayout;
			if (textures[i]->format >= LGFXTextureFormat_Depth16Unorm)
			{
				newLayout = LGFXTextureLayout_DepthStencilAttachmentOptimal;
			}
			else
			{
				newLayout = LGFXTextureLayout_ColorAttachmentOptimal;
			}
			VkLGFXTextureTransitionLayout(program->device, outputTarget->textures[i], newLayout, commandBuffer, 0, 1);
		}
		//VkLGFXEndTemporaryCommandBuffer(program->device, tempBuffer);
	}

	VkRenderPassBeginInfo info = {0};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.framebuffer = (VkFramebuffer)outputTarget->handle;
	info.renderPass = (VkRenderPass)program->handle;

	//clear values
	//arbitrarily large number
	VkClearValue clearValues[32];

	info.clearValueCount = program->attachmentsCount; // this->currentRenderProgram->attachments.count;
	info.pClearValues = clearValues;
	for (u32 i = 0; i < program->attachmentsCount; i++)
	{
		VkClearValue clearValue = {0};
		LGFXRenderAttachmentInfo *attachment = &program->attachments[i];

		if (attachment->clear)
		{
			if (attachment->format < LGFXTextureFormat_Depth16Unorm)
			{
				clearValues[i].color.float32[0] = clearColor.R * ONE_OVER_255;
				clearValues[i].color.float32[1] = clearColor.G * ONE_OVER_255;
				clearValues[i].color.float32[2] = clearColor.B * ONE_OVER_255;
				clearValues[i].color.float32[3] = clearColor.A * ONE_OVER_255;
			}
			else
			{
				clearValues[i].depthStencil.depth = 1.0f;
				clearValues[i].depthStencil.stencil = 255;
			}
		}
	}

	info.renderArea.offset.x = 0;
	info.renderArea.offset.y = 0;
	info.renderArea.extent.width = outputTarget->textures[0]->width;
	info.renderArea.extent.height = outputTarget->textures[0]->height;

	vkCmdBeginRenderPass((VkCommandBuffer)commandBuffer->cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
}
void VkLGFXRenderProgramNextPass(LGFXCommandBuffer commandBuffer)
{
	vkCmdNextSubpass((VkCommandBuffer)commandBuffer->cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
}
void VkLGFXEndRenderProgram(LGFXRenderProgram program, LGFXCommandBuffer commandBuffer)
{
	vkCmdEndRenderPass((VkCommandBuffer)commandBuffer->cmdBuffer);

	if (program->currentTarget != NULL)
	{
		LGFXTexture *textures = program->currentTarget->textures;
		for (u32 i = 0; i < program->attachmentsCount; i++)
		{
			if (program->attachments[i].format < LGFXTextureFormat_Depth16Unorm)
			{
				if (!program->attachments[i].readByRenderTarget)
				{
					textures[i]->layout = LGFXTextureLayout_PresentSource;
					//createInfo.textures[i] = outputSwapchain->backbufferTextures[index];
				}
				else
				{
					textures[i]->layout = LGFXTextureLayout_ShaderReadOptimal;
				}
			}
			else
			{
				if (!program->attachments[i].readByRenderTarget)
				{
					textures[i]->layout = LGFXTextureLayout_DepthStencilAttachmentOptimal;
				}
				else
				{
					textures[i]->layout = LGFXTextureLayout_ShaderReadOptimal; // LGFXTextureLayout_DepthStencilReadOptimal;
				}
				//createInfo.textures[i] = outputSwapchain->backDepthbuffers[index]; // LGFXCreateTexture(program->device, &textureInfo);
			}
		}
	}
	program->currentTarget = NULL;
}

LGFXFunction VkLGFXCreateFunction(LGFXDevice device, LGFXFunctionCreateInfo *info)
{
	VkShaderModuleCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = info->module1DataLength * 4;
	createInfo.pCode = info->module1Data;

	VkShaderModule shaderModule1 = NULL;
	VkShaderModule shaderModule2 = NULL;

	if (vkCreateShaderModule((VkDevice)device->logicalDevice, &createInfo, NULL, &shaderModule1) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create shader module 1\n");
		return NULL;
	}

	if (info->module2Data != NULL && info->module2DataLength > 0)
	{
		createInfo.codeSize = info->module2DataLength * 4;
		createInfo.pCode = info->module2Data;

		if (vkCreateShaderModule((VkDevice)device->logicalDevice, &createInfo, NULL, &shaderModule2) != VK_SUCCESS)
		{
			LGFX_ERROR("Failed to create shader module 2\n");
			return NULL;
		}
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.flags = 0;
	layoutInfo.bindingCount = info->uniformsCount;
	VkDescriptorSetLayoutBinding *bindings = NULL;
	if (info->uniformsCount > 0)
	{
		bindings = Allocate(VkDescriptorSetLayoutBinding, info->uniformsCount);
		for (u32 i = 0; i < info->uniformsCount; i++)
		{
			VkDescriptorSetLayoutBinding layoutBinding = {0};
			layoutBinding.binding = info->uniforms[i].binding;
			layoutBinding.descriptorCount = max(info->uniforms[i].arrayLength, 1);
			layoutBinding.descriptorType = LGFXShaderResourceType2Vulkan(info->uniforms[i].type);
			layoutBinding.stageFlags = LGFXShaderInputAccess2Vulkan(info->uniforms[i].accessedBy);
			layoutBinding.pImmutableSamplers = NULL;

			bindings[layoutBinding.binding] = layoutBinding;
		}
	}
	layoutInfo.pBindings = bindings;

	VkDescriptorSetLayout descriptorLayout;
	if (vkCreateDescriptorSetLayout((VkDevice)device->logicalDevice, &layoutInfo, NULL, &descriptorLayout) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create pipeline layout\n");
		return NULL;
	}

	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {0};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
	pipelineLayoutCreateInfo.flags = 0;

	pipelineLayoutCreateInfo.setLayoutCount = 0;
	if (descriptorLayout != NULL)
	{
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = (VkDescriptorSetLayout*)&descriptorLayout;
	}

	if (vkCreatePipelineLayout((VkDevice)device->logicalDevice, &pipelineLayoutCreateInfo, NULL, (VkPipelineLayout*)&pipelineLayout) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to initialize pipeline layout");
	}

	LGFXFunction result = Allocate(LGFXFunctionImpl, 1);
	result->module1 = shaderModule1;
	result->module2 = shaderModule2;
	if (info->uniformsCount > 0)
	{
		result->uniforms = Allocate(LGFXShaderResource, info->uniformsCount);
		for (u32 i = 0; i < info->uniformsCount; i++)
		{
			result->uniforms[i] = info->uniforms[i];
		}
	}
	else
	{
		result->uniforms = NULL;
	}
	result->uniformsCount = info->uniformsCount;
	result->functionVariablesLayout = descriptorLayout;
	result->device = device;
	result->pipelineLayout = pipelineLayout;
	result->type = info->type;

	free(bindings);

	return result;
}
LGFXFunctionVariableBatch VkLGFXFunctionGetVariableBatch(LGFXFunction function)
{
	VkDescriptorSetAllocateInfo allocInfo = {0};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorSetCount = 1;
	allocInfo.descriptorPool = (VkDescriptorPool)function->device->descriptorPool;
	allocInfo.pSetLayouts = (VkDescriptorSetLayout*)&function->functionVariablesLayout;

	VkDescriptorSet set;
	i32 errorCode = vkAllocateDescriptorSets((VkDevice)function->device->logicalDevice, &allocInfo, &set);
	if (errorCode != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to allocate descriptor set, error code %i\n", errorCode);
		return NULL;
	}

	return (LGFXFunctionVariableBatch)set;
}
LGFXFunctionVariable VkLGFXFunctionGetVariableSlot(LGFXFunction function, u32 forVariableOfIndex)
{
	LGFXFunctionVariable variable = {0};
	LGFXShaderResource *resource = &function->uniforms[forVariableOfIndex];
	variable.metadata = resource;
	variable.valuesCount = resource->arrayLength == 0 ? 1 : resource->arrayLength;
	variable.device = function->device;
	variable.forFunction = function;

	switch (resource->type)
	{
		case LGFXShaderResourceType_StructuredBuffer:
		case LGFXShaderResourceType_Uniform:
		{
			variable.currentValues = (void **)Allocate(LGFXBuffer, variable.valuesCount);
			variable.infos = (void **)Allocate(VkDescriptorBufferInfo, variable.valuesCount);

			if (resource->type == LGFXShaderResourceType_Uniform)
			{
				variable.valueIsOwnedBuffer = true;
				LGFXBufferCreateInfo info;
				info.size = variable.metadata->size;
				info.bufferUsage = LGFXBufferUsage_UniformBuffer;
				info.memoryUsage = LGFXMemoryUsage_CPU_TO_GPU;
				((LGFXBuffer *)variable.currentValues)[0] = LGFXCreateBuffer(function->device, &info);
			}
			else
			{
				variable.valueIsOwnedBuffer = false;
			}
			break;
		}
		case LGFXShaderResourceType_StorageTexture:
		case LGFXShaderResourceType_Texture:
		{
			variable.currentValues = (void **)Allocate(LGFXTexture, variable.valuesCount);
			variable.infos = (void **)Allocate(VkDescriptorImageInfo, variable.valuesCount);
			break;
		}
		case LGFXShaderResourceType_Sampler:
		{
			variable.currentValues = (void **)Allocate(LGFXSamplerState, variable.valuesCount);
			variable.infos = (void **)Allocate(VkDescriptorImageInfo, variable.valuesCount);
			break;
		}
		default:
		{
			break;
		}
	}
	return variable;
}
void VkLGFXFunctionSendVariablesToGPU(LGFXDevice device, LGFXFunctionVariableBatch batch, LGFXFunctionVariable *shaderVariables, u32 shaderVariableCount)
{
	VkWriteDescriptorSet setWrites[32];

	for (u32 i = 0; i < shaderVariableCount; i++)
	{
		VkWriteDescriptorSet setWrite = {0};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.dstSet = (VkDescriptorSet)batch;
		setWrite.dstBinding = shaderVariables[i].metadata->binding;

		switch (shaderVariables[i].metadata->type)
		{
			case LGFXShaderResourceType_InputAttachment:
			{
				VkDescriptorImageInfo imageInfo = {0};
				imageInfo.sampler = NULL;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
				imageInfo.imageView = (VkImageView)(((LGFXTexture)shaderVariables[i].currentValues[0])->imageView); // toMutate->textures.data[0]->imageView;
				((VkDescriptorImageInfo*)shaderVariables[i].infos)[0] = imageInfo;

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = shaderVariables[i].valuesCount;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				setWrite.pImageInfo = (VkDescriptorImageInfo *)shaderVariables[i].infos;
				break;
			}
			case LGFXShaderResourceType_Uniform:
			{
				LGFXBuffer buffer = (LGFXBuffer)shaderVariables[i].currentValues[0];
				VkDescriptorBufferInfo bufferInfo = {0};
				bufferInfo.buffer = (VkBuffer)buffer->handle;
				bufferInfo.offset = 0;
				bufferInfo.range = buffer->size;
				*((VkDescriptorBufferInfo *)shaderVariables[i].infos) = bufferInfo;

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = 1;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				setWrite.pBufferInfo = (VkDescriptorBufferInfo *)shaderVariables[i].infos;

				break;
			}
			case LGFXShaderResourceType_Texture:
			{
				for (u32 j = 0; j < shaderVariables[i].valuesCount; j++)
				{
					VkDescriptorImageInfo imageInfo = {0};
					imageInfo.sampler = NULL;
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
					imageInfo.imageView = (VkImageView)(((LGFXTexture *)shaderVariables[i].currentValues)[j]->imageView);
					((VkDescriptorImageInfo*)shaderVariables[i].infos)[j] = imageInfo;
				}

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = shaderVariables[i].valuesCount;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				setWrite.pImageInfo = (VkDescriptorImageInfo *)shaderVariables[i].infos;

				break;
			}
			case LGFXShaderResourceType_Sampler:
			{
				for (u32 j = 0; j < shaderVariables[i].valuesCount; j++)
				{
					VkDescriptorImageInfo imageInfo = {0};
					imageInfo.sampler = (VkSampler)(((LGFXSamplerState *)shaderVariables[i].currentValues)[j]->handle);
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
					imageInfo.imageView = NULL;
					((VkDescriptorImageInfo*)shaderVariables[i].infos)[j] = imageInfo;
				}

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = shaderVariables[i].valuesCount;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				setWrite.pImageInfo = (VkDescriptorImageInfo *)shaderVariables[i].infos;

				break;
			}
			case LGFXShaderResourceType_StructuredBuffer:
			{
				LGFXBuffer buffer = (LGFXBuffer)shaderVariables[i].currentValues[0];
				VkDescriptorBufferInfo bufferInfo = {0};
				bufferInfo.buffer = (VkBuffer)buffer->handle;
				bufferInfo.offset = 0;
				bufferInfo.range = buffer->size;
				*((VkDescriptorBufferInfo *)shaderVariables[i].infos) = bufferInfo;

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = 1;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				setWrite.pBufferInfo = (VkDescriptorBufferInfo *)shaderVariables[i].infos;

				break;
			}
			case LGFXShaderResourceType_StorageTexture:
			{
				LGFXTexture texture = (LGFXTexture)shaderVariables[i].currentValues[0];
				VkDescriptorImageInfo textureInfo = {0};
				//compute textures use the general layout format
				textureInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				textureInfo.imageView = (VkImageView)texture->imageView;
				textureInfo.sampler = NULL;
				*((VkDescriptorImageInfo *)shaderVariables[i].infos) = textureInfo;

				setWrite.dstArrayElement = 0;
				setWrite.descriptorCount = 1;
				setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				setWrite.pImageInfo = (VkDescriptorImageInfo *)shaderVariables[i].infos;

				break;
			}
			default:
				break;
		}
		setWrites[i] = setWrite;
	}

	vkUpdateDescriptorSets((VkDevice)device->logicalDevice, shaderVariableCount, setWrites, 0, NULL);
}
void VkLGFXUseFunctionVariables(LGFXCommandBuffer commandBuffer, LGFXFunctionVariableBatch batch, LGFXFunction forFunction)
{
	VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	if (forFunction->type == LGFXFunctionType_Compute)
	{
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	}
	// for (u32 i = 0; i < variablesCount; i++)
	// {
	vkCmdBindDescriptorSets(
		(VkCommandBuffer)commandBuffer->cmdBuffer, 
		bindPoint, 
		(VkPipelineLayout)forFunction->pipelineLayout, 
		0, 1, //descriptor set count
		(VkDescriptorSet *)&batch,
		0, NULL); //dynamic offsets count
	//}
	// VkDescriptorSet sets[32];
	// for (u32 i = 0; i < variablesCount; i++)
	// {
	// 	sets[variables[i].metadata->binding] = (VkDescriptorSet)variables[i].descriptor;
	// }

	// VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	// if (variables[0].forFunction->type == LGFXFunctionType_Compute)
	// {
	// 	bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	// }
	
	// vkCmdBindDescriptorSets(
	// 	(VkCommandBuffer)commandBuffer->cmdBuffer, 
	// 	bindPoint, 
	// 	(VkPipelineLayout)variables[0].forFunction->pipelineLayout, 
	// 	0, 1, //descriptor set count
	// 	sets,
	// 	0, NULL); //dynamic offsets count
}

LGFXShaderState VkLGFXCreateShaderState(LGFXDevice device, LGFXShaderStateCreateInfo *info)
{
	VkPipeline result;
	if (info->function->type == LGFXFunctionType_Compute)
	{
		VkPipelineShaderStageCreateInfo shaderStageInfo = {0};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageInfo.module = (VkShaderModule)info->function->module1;
		shaderStageInfo.pName = "main"; //entry point

		VkComputePipelineCreateInfo computeInfo = {0};
		computeInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		computeInfo.stage = shaderStageInfo;
		computeInfo.layout = info->function->pipelineLayout;

		if (vkCreateComputePipelines((VkDevice)device->logicalDevice, NULL, 1, &computeInfo, NULL, &result) != VK_SUCCESS)
		{
			LGFX_ERROR("Failed to create compute shader state\n");
		}
	}
	else
	{
		//dynamic state
		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {0};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		VkDynamicState dynamicStates[3];
		u32 dynamicStatesCount = 0;
		if (info->dynamicLineWidth)
		{
			dynamicStates[dynamicStatesCount] = VK_DYNAMIC_STATE_LINE_WIDTH;
			dynamicStatesCount += 1;
		}
		if (info->dynamicViewportScissor)
		{
			dynamicStates[dynamicStatesCount] = VK_DYNAMIC_STATE_SCISSOR;
			dynamicStatesCount += 1;
			dynamicStates[dynamicStatesCount] = VK_DYNAMIC_STATE_VIEWPORT;
			dynamicStatesCount += 1;
		}
		dynamicStateInfo.pDynamicStates = dynamicStates;
		dynamicStateInfo.dynamicStateCount = dynamicStatesCount;

		//vertex

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;

		if (info->vertexDeclarationCount > 0)
		{
			u32 attribCount = 0;

			VkVertexInputBindingDescription *bindingDescriptions = Allocate(VkVertexInputBindingDescription, info->vertexDeclarationCount);
			for (u32 i = 0; i < info->vertexDeclarationCount; i++)
			{
				bindingDescriptions[i].inputRate = info->vertexDeclarations[i].isPerInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX; //(VkVertexInputRate)pipeline->vertexDeclarations.data[i]->inputRate;
				bindingDescriptions[i].stride = info->vertexDeclarations[i].packedSize;
				bindingDescriptions[i].binding = i;

				attribCount += info->vertexDeclarations[i].elementsCount; // pipeline->vertexDeclarations.data[i]->elements.count;
			}

			VkVertexInputAttributeDescription *attribDescriptions = Allocate(VkVertexInputAttributeDescription, attribCount);
			u32 attribIndex = 0;
			for (u32 i = 0; i < info->vertexDeclarationCount; i++)
			{
				for (u32 j = 0; j < info->vertexDeclarations[i].elementsCount; j++)
				{
					LGFXVertexAttribute element = info->vertexDeclarations[i].elements[j];
					//AstralCanvas::VertexElement element = pipeline->vertexDeclarations.data[i]->elements.ptr[j];

					attribDescriptions[attribIndex].format = LGFXVertexElementFormat2Vulkan(element.format);
					attribDescriptions[attribIndex].binding = i;
					attribDescriptions[attribIndex].offset = element.offset;
					attribDescriptions[attribIndex].location = attribIndex; //very important!!
					attribIndex++;
				}
			}

			vertexInputInfo.vertexBindingDescriptionCount = info->vertexDeclarationCount;
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
			vertexInputInfo.vertexAttributeDescriptionCount = attribCount;
			vertexInputInfo.pVertexAttributeDescriptions = attribDescriptions;
		}

		//primitive type

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {0};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = LGFXPrimitiveType2Vulkan(info->primitiveType); // AstralCanvasVk_FromPrimitiveType(pipeline->primitiveType);
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		//viewport data

		VkPipelineViewportStateCreateInfo viewportStateInfo = {0};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		//rasterization behaviour

		VkPipelineRasterizationStateCreateInfo rasterizerInfo = {0};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = false;
		rasterizerInfo.rasterizerDiscardEnable = false;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		if (inputAssemblyInfo.topology == VK_PRIMITIVE_TOPOLOGY_LINE_LIST || inputAssemblyInfo.topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)
		{
			rasterizerInfo.polygonMode = VK_POLYGON_MODE_LINE;
		}
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = (VkCullModeFlags)info->cullMode;
		rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		rasterizerInfo.depthBiasEnable = false;
		rasterizerInfo.depthBiasConstantFactor = 0.0f;
		rasterizerInfo.depthBiasClamp = 0.0f;
		rasterizerInfo.depthBiasSlopeFactor = 0.0f;

		//multisampling data
		//todo

		VkPipelineMultisampleStateCreateInfo multisamplingInfo = {0};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = false;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingInfo.minSampleShading = 1.0f;
		multisamplingInfo.pSampleMask = NULL;
		multisamplingInfo.alphaToCoverageEnable = false;
		multisamplingInfo.alphaToOneEnable = false;

		//depth stencil data

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {0};
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = info->depthTest;
		depthStencilInfo.depthWriteEnable = info->depthWrite;
		depthStencilInfo.depthCompareOp = info->depthTest ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_ALWAYS;
		depthStencilInfo.depthBoundsTestEnable = false;
		depthStencilInfo.minDepthBounds = 0.0f;
		depthStencilInfo.maxDepthBounds = 1.0f;
		depthStencilInfo.stencilTestEnable = false;

		//color blend state

		VkPipelineColorBlendAttachmentState colorBlendState = {0};
		colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // ColorComponentFlags.RBit | ColorComponentFlags.GBit | ColorComponentFlags.BBit | ColorComponentFlags.ABit;

		LGFXBlendState disabled = DISABLE_BLEND;
		if (!LGFXBlendStateEquals(info->blendState, disabled))
		{
			colorBlendState.srcColorBlendFactor = LGFXBlendState2Vulkan(info->blendState.sourceColorBlend);
			colorBlendState.srcAlphaBlendFactor = LGFXBlendState2Vulkan(info->blendState.sourceAlphaBlend);
			colorBlendState.dstColorBlendFactor = LGFXBlendState2Vulkan(info->blendState.destinationColorBlend);
			colorBlendState.dstAlphaBlendFactor = LGFXBlendState2Vulkan(info->blendState.destinationAlphaBlend);
			colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
			colorBlendState.blendEnable = true;
		}
		else
			colorBlendState.blendEnable = false;

		//color blend data

		VkPipelineColorBlendStateCreateInfo colorBlendInfo = {0};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.blendConstants[0] = 1.0f;
		colorBlendInfo.blendConstants[1] = 1.0f;
		colorBlendInfo.blendConstants[2] = 1.0f;
		colorBlendInfo.blendConstants[3] = 1.0f;
		colorBlendInfo.logicOpEnable = false;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendState;

		//pipeline layout itself
		//dont reuse these

		VkPipelineShaderStageCreateInfo shaderStageInfos[2];
		memset(shaderStageInfos, 0, sizeof(VkPipelineShaderStageCreateInfo) * 2);
		shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStageInfos[0].module = (VkShaderModule)info->function->module1;
		shaderStageInfos[0].pName = "main"; //entry point

		shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStageInfos[1].module = (VkShaderModule)info->function->module2;
		shaderStageInfos[1].pName = "main"; //entry point

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {0};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pStages = shaderStageInfos;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineCreateInfo.pViewportState = &viewportStateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
		pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
		pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateInfo;
		pipelineCreateInfo.layout = (VkPipelineLayout)info->function->pipelineLayout;
		pipelineCreateInfo.renderPass = (VkRenderPass)info->forRenderProgram->handle;
		pipelineCreateInfo.subpass = info->forRenderPass;

		if (vkCreateGraphicsPipelines((VkDevice)device->logicalDevice, NULL, 1, &pipelineCreateInfo, NULL, &result) != VK_SUCCESS)
		{
			LGFX_ERROR("Error creating pipeline\n");
		}
	}

	LGFXShaderState shader = Allocate(LGFXShaderStateImpl, 1);
	shader->device = device;
	shader->handle = result;
	shader->function = info->function;

	return shader;
}
void VkLGFXUseShaderState(LGFXCommandBuffer buffer, LGFXShaderState shaderState)
{
	VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	if (shaderState->function->type == LGFXFunctionType_Compute)
	{
		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	}
	vkCmdBindPipeline((VkCommandBuffer)buffer->cmdBuffer, bindPoint, (VkPipeline)shaderState->handle);
}

void VkLGFXSetClipArea(LGFXCommandBuffer commandBuffer, LGFXRectangle area)
{
	VkRect2D rects;
	rects.offset.x = area.X;
	rects.offset.y = area.Y;
	rects.extent.width = area.width;
	rects.extent.height = area.height;
	vkCmdSetScissor((VkCommandBuffer)commandBuffer->cmdBuffer, 0, 1, &rects);
}
void VkLGFXSetViewport(LGFXCommandBuffer commandBuffer, LGFXBox area)
{
	VkViewport viewports;
	viewports.minDepth = 0.0f;
	viewports.maxDepth = 1.0f;
	viewports.x = area.X;
	viewports.y = area.Y;
	viewports.width = area.width;
	viewports.height = area.height;
	vkCmdSetViewport((VkCommandBuffer)commandBuffer->cmdBuffer, 0, 1, &viewports);
}

LGFXCommandBuffer VkLGFXCreateCommandBuffer(LGFXDevice device, bool forCompute)
{
	LGFXCommandBuffer result = Allocate(LGFXCommandBufferImpl, 1);
	result->queue = forCompute ? device->computeQueue : device->graphicsQueue;

	EnterLock(result->queue->commandPoolLock);

	VkCommandBufferAllocateInfo cmdBufferInfo = {0};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.commandPool = (VkCommandPool)result->queue->regularCommandPool;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = 1;
	VkResult opResult = vkAllocateCommandBuffers((VkDevice)device->logicalDevice, &cmdBufferInfo, (VkCommandBuffer *)&result->cmdBuffer);

	ExitLock(result->queue->commandPoolLock);

	if (opResult != VK_SUCCESS)
	{
		free(result);
		return NULL;
	}

	return result;
}
void VkLGFXCommandBufferBegin(LGFXCommandBuffer buffer, bool resetAfterSubmission)
{
	VkCommandBufferBeginInfo beginInfo = {0};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pInheritanceInfo = NULL;
	beginInfo.pNext = NULL;
	beginInfo.flags = resetAfterSubmission ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : NULL;

	if (vkBeginCommandBuffer((VkCommandBuffer)buffer->cmdBuffer, &beginInfo) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to begin command buffer\n");
	}

	buffer->begun = true;
}
void VkLGFXCommandBufferEndSwapchain(LGFXCommandBuffer buffer, LGFXSwapchain swapchain)
{
	VkLGFXCommandBufferEnd(buffer);

	EnterLock(buffer->queue->inDevice->graphicsQueue->queueLock);

	vkQueueWaitIdle((VkQueue)buffer->queue->inDevice->graphicsQueue->queue);

	ExitLock(buffer->queue->inDevice->graphicsQueue->queueLock);

	VkLGFXCommandBufferExecute(buffer, swapchain->fence, swapchain->awaitPresentComplete, swapchain->awaitRenderComplete);
}

void VkLGFXCommandBufferEnd(LGFXCommandBuffer buffer)
{
	LGFXDevice device = buffer->queue->inDevice;
	vkEndCommandBuffer((VkCommandBuffer)buffer->cmdBuffer);

	buffer->begun = false;
}
void VkLGFXCommandBufferExecute(LGFXCommandBuffer buffer, LGFXFence fence, LGFXSemaphore awaitSemaphore, LGFXSemaphore signalSemaphore)
{
	VkCommandBufferSubmitInfo commandBufferInfo = {0};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferInfo.commandBuffer = (VkCommandBuffer)buffer->cmdBuffer;

	VkSubmitInfo2 submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.commandBufferInfoCount = 1; // commandBufferCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferInfo;

	VkSemaphoreSubmitInfo awaitSemaphoreInfo = {0};
	VkSemaphoreSubmitInfo signalSemaphoreInfo = {0};

	if (awaitSemaphore != NULL)
	{
		awaitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		awaitSemaphoreInfo.semaphore = (VkSemaphore)awaitSemaphore->semaphore;
		awaitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		submitInfo.waitSemaphoreInfoCount = 1;
		submitInfo.pWaitSemaphoreInfos = &awaitSemaphoreInfo;
		//submitInfo.waitSemaphoreCount = 1;
		//submitInfo.pWaitSemaphores = (VkSemaphore *)&awaitSemaphore->semaphore;
	}
	if (signalSemaphore != NULL)
	{
		signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signalSemaphoreInfo.semaphore = (VkSemaphore)signalSemaphore->semaphore;
		signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;

		submitInfo.signalSemaphoreInfoCount = 1;
		submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo;
		// submitInfo.signalSemaphoreCount = 1;
		// submitInfo.pSignalSemaphores = (VkSemaphore *)&signalSemaphore->semaphore;
	}

	//if we are on the graphics queue, the queue may also be used for presenting
    //hence, we need to wait for the queue to finish presenting before we can transition the image
    /*if (buffer->queue == buffer->queue->inDevice->graphicsQueue)
    {
		EnterLock(buffer->queue->inDevice->graphicsQueue->queueLock);

		vkQueueWaitIdle((VkQueue)buffer->queue->inDevice->graphicsQueue->queue);

		ExitLock(buffer->queue->inDevice->graphicsQueue->queueLock);
	}*/

	//submit the queue
    EnterLock(buffer->queue->queueLock);

	VkResult submitResult = vkQueueSubmit2((VkQueue)buffer->queue->queue, 1, &submitInfo, fence == NULL ? NULL : (VkFence)fence->fence);
	if (submitResult != VK_SUCCESS)
    {
		LGFX_ERROR("Failed to submit queue, error code %u\n", submitResult);
    }
	ExitLock(buffer->queue->queueLock);
}
void VkLGFXCommandBufferReset(LGFXCommandBuffer buffer)
{
	vkResetCommandBuffer((VkCommandBuffer)buffer->cmdBuffer, 0);
}

LGFXSamplerState VkLGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info)
{
	VkSamplerCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.addressModeU = LGFXSamplerRepeatMode2Vulkan(info->repeatModeU);
	createInfo.addressModeV = LGFXSamplerRepeatMode2Vulkan(info->repeatModeV);
	createInfo.addressModeW = LGFXSamplerRepeatMode2Vulkan(info->repeatModeW);
	createInfo.anisotropyEnable = info->isAnisotropic;
	createInfo.maxAnisotropy = info->maxAnisotropy;
	createInfo.minFilter = LGFXFilterType2Vulkan(info->minimizationFilter);
	createInfo.magFilter = LGFXFilterType2Vulkan(info->magnificationFilter);
	createInfo.minLod = info->minLODClamp;
	createInfo.maxLod = info->maxLODClamp;
	createInfo.borderColor = LGFXSamplerBorderColor2Vulkan(info->borderColor);

	LGFXSamplerState state = Allocate(LGFXSamplerStateImpl, 1);
	state->device = device;
	if (vkCreateSampler((VkDevice)device->logicalDevice, &createInfo, NULL, (VkSampler *)&state->handle) != VK_SUCCESS)
	{
		LGFX_ERROR("Failed to create sampler state\n");
		free(state);
		return NULL;
	}

	return state;
}
void VkLGFXDestroySamplerState(LGFXSamplerState state)
{
	if (state->handle != NULL)
	{
		vkDestroySampler((VkDevice)state->device->logicalDevice, (VkSampler)state->handle, NULL);
	}
	free(state);
}

void VkLGFXUseIndexBuffer(LGFXCommandBuffer commands, LGFXBuffer indexBuffer, usize offset)
{
	vkCmdBindIndexBuffer((VkCommandBuffer)commands->cmdBuffer, (VkBuffer)indexBuffer->handle, offset, VK_INDEX_TYPE_UINT32);
}
void VkLGFXUseVertexBuffer(LGFXCommandBuffer commands, LGFXBuffer *vertexBuffers, u32 vertexBuffersCount)
{
	VkBuffer vkBuffers[16];
	VkDeviceSize offsets[16];
	for (u32 i = 0; i < vertexBuffersCount; i++)
	{
		vkBuffers[i] = (VkBuffer)vertexBuffers[i]->handle;
		offsets[i] = 0;
	}
	vkCmdBindVertexBuffers((VkCommandBuffer)commands->cmdBuffer, 0, vertexBuffersCount, vkBuffers, offsets);
}
void VkLGFXDrawIndexed(LGFXCommandBuffer commands, u32 indexCount, u32 instances, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
{
	vkCmdDrawIndexed((VkCommandBuffer)commands->cmdBuffer, indexCount, instances, firstIndex, vertexOffset, firstInstance);
}
void VkLGFXDrawIndexedIndirect(LGFXCommandBuffer commands, LGFXBuffer drawParamsBuffer, usize bufferOffset, usize drawCount, usize drawParamsStride)
{
	if (drawParamsStride == 0)
	{
		drawParamsStride = sizeof(VkDrawIndexedIndirectCommand);
	}
	vkCmdDrawIndexedIndirect((VkCommandBuffer)commands->cmdBuffer, (VkBuffer)drawParamsBuffer->handle, bufferOffset, (u32)drawCount, (u32)drawParamsStride);
}

void VkLGFXDispatchCompute(LGFXCommandBuffer commands, u32 groupsX, u32 groupsY, u32 groupsZ)
{
	vkCmdDispatch((VkCommandBuffer)commands->cmdBuffer, groupsX, groupsY, groupsZ);
}
void VkLGFXDispatchComputeIndirect(LGFXCommandBuffer commands, LGFXBuffer dispatchParamsBuffer, usize offset)
{
	vkCmdDispatchIndirect((VkCommandBuffer)commands->cmdBuffer, (VkBuffer)dispatchParamsBuffer->handle, offset);
}
// END

// DESTROY FUNCTIONS
void VkLGFXDestroyFunctionVariable(LGFXFunctionVariable variable)
{
	if (variable.valueIsOwnedBuffer)
	{
		LGFXBuffer buffer = (LGFXBuffer)variable.currentValues[0];
		LGFXDestroyBuffer(buffer);
	}
	free(variable.infos);
	free(variable.currentValues);
}
void VkLGFXDestroyFence(LGFXFence fence)
{
	vkDestroyFence((VkDevice)fence->device->logicalDevice, (VkFence)fence->fence, NULL);
	free(fence);
}
void VkLGFXDestroySemaphore(LGFXSemaphore semaphore)
{
	vkDestroySemaphore((VkDevice)semaphore->device->logicalDevice, (VkSemaphore)semaphore->semaphore, NULL);
	free(semaphore);
}
void VkLGFXDestroyCommandBuffer(LGFXCommandBuffer commandBuffer)
{
	if (commandBuffer->cmdBuffer != NULL)
	{
		EnterLock(commandBuffer->queue->commandPoolLock);
		vkFreeCommandBuffers((VkDevice)commandBuffer->queue->inDevice->logicalDevice, (VkCommandPool)commandBuffer->queue->regularCommandPool, 1, (VkCommandBuffer *)&commandBuffer->cmdBuffer);
		ExitLock(commandBuffer->queue->commandPoolLock);
	}
	free(commandBuffer);
}
void VkLGFXDestroyShaderState(LGFXShaderState shaderState)
{
	if (shaderState->handle != NULL)
	{
		vkDestroyPipeline((VkDevice)shaderState->device->logicalDevice, (VkPipeline)shaderState->handle, NULL);
	}
	free(shaderState);
}
void VkLGFXDestroyFunction(LGFXFunction func)
{
	if (func->pipelineLayout != NULL)
	{
		vkDestroyPipelineLayout((VkDevice)func->device->logicalDevice, (VkPipelineLayout)func->pipelineLayout, NULL);
	}
	if (func->module1 != NULL)
	{
		vkDestroyShaderModule((VkDevice)func->device->logicalDevice, (VkShaderModule)func->module1, NULL);
	}
	if (func->module2 != NULL)
	{
		vkDestroyShaderModule((VkDevice)func->device->logicalDevice, (VkShaderModule)func->module2, NULL);
	}
	if (func->functionVariablesLayout != NULL)
	{
		vkDestroyDescriptorSetLayout((VkDevice)func->device->logicalDevice, (VkDescriptorSetLayout)func->functionVariablesLayout, NULL);
	}
	if (func->uniforms != NULL)
	{
		free(func->uniforms);
	}
	free(func);
}
void VkLGFXDestroyRenderProgram(LGFXRenderProgram program)
{
	if (program->targetsCount > 0)
	{
		for (u32 i = 0; i < program->targetsCount; i++)
		{
			if (program->targets[i] != NULL)
			{
				VkLGFXDestroyRenderTarget(program->targets[i]);
			}
		}
		free(program->targets);
	}
	vkDestroyRenderPass((VkDevice)program->device->logicalDevice, (VkRenderPass)program->handle, NULL);

	free(program);
}
void VkLGFXDestroyRenderTarget(LGFXRenderTarget target)
{
	vkDestroyFramebuffer((VkDevice)target->device->logicalDevice, (VkFramebuffer)target->handle, NULL);

	free(target->textures);
	free(target);
}
void VkLGFXDestroyBuffer(LGFXBuffer buffer)
{
	vkDestroyBuffer((VkDevice)buffer->device->logicalDevice, (VkBuffer)buffer->handle, NULL);

	vmaFreeMemory((VmaAllocator)buffer->device->memoryAllocator, buffer->bufferMemory->vkAllocation);

	free(buffer->bufferMemory);
	free(buffer);
}
void VkLGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed)
{
	EnterLock(swapchain->device->graphicsQueue->queueLock);
	vkQueueWaitIdle((VkQueue)swapchain->device->graphicsQueue->queue);
	ExitLock(swapchain->device->graphicsQueue->queueLock);
	vkDeviceWaitIdle((VkDevice)swapchain->device->logicalDevice);
	if (swapchain->swapchain != NULL)
	{
		for (u32 i = 0; i < swapchain->backbufferTexturesCount; i++)
		{
			LGFXDestroyTexture(swapchain->backbufferTextures[i]);
			LGFXDestroyTexture(swapchain->backDepthbuffers[i]);
		}
		free(swapchain->backbufferTextures);
		free(swapchain->backDepthbuffers);

		vkDestroySwapchainKHR((VkDevice)swapchain->device->logicalDevice, (VkSwapchainKHR)swapchain->swapchain, NULL);
	}
	if (swapchain->awaitPresentComplete != NULL)
	{
		LGFXDestroySemaphore(swapchain->awaitPresentComplete);
		LGFXDestroySemaphore(swapchain->awaitRenderComplete);
		LGFXDestroyFence(swapchain->fence);
	}
	if (swapchain->windowSurface != NULL && windowIsDestroyed)
	{
		vkDestroySurfaceKHR((VkInstance)swapchain->device->instance->instance, (VkSurfaceKHR)swapchain->windowSurface, NULL);
	}
	free(swapchain);
}
void VkLGFXDestroyCommandQueue(LGFXDevice device, LGFXCommandQueue queue)
{
	if (queue->transientCommandPool != NULL)
	{
		vkDestroyCommandPool((VkDevice)device->logicalDevice, (VkCommandPool)queue->transientCommandPool, NULL);
	}
	if (queue->regularCommandPool != NULL)
	{
		vkDestroyCommandPool((VkDevice)device->logicalDevice, (VkCommandPool)queue->regularCommandPool, NULL);
	}
	if (queue->fence != NULL)
	{
		VkLGFXDestroyFence(queue->fence);
	}
	if (queue->queueLock != NULL)
	{
		DestroyLock(queue->queueLock);
	}
	if (queue->commandPoolLock != NULL)
	{
		DestroyLock(queue->commandPoolLock);
	}
	free(queue);
}
void VkLGFXDestroyDevice(LGFXDevice device)
{
	if (device->logicalDevice != NULL)
	{
		if (device->computeQueue != device->graphicsQueue)
		{
			VkLGFXDestroyCommandQueue(device, device->computeQueue);
		}

		if (device->transferQueue != device->graphicsQueue)
		{
			VkLGFXDestroyCommandQueue(device, device->transferQueue);
		}

		vkDestroyDescriptorPool((VkDevice)device->logicalDevice, (VkDescriptorPool)device->descriptorPool, NULL);

		VkLGFXDestroyCommandQueue(device, device->graphicsQueue);

		vmaDestroyAllocator((VmaAllocator)device->memoryAllocator);

		vkDestroyDevice((VkDevice)device->logicalDevice, NULL);
	}
}
void VkLGFXDestroyInstance(LGFXInstance instance)
{
	if (instance->instance != NULL)
	{
		vkDestroyInstance((VkInstance)instance->instance, NULL);
	}
	if (instance->enabledErrorCheckerExtensions != NULL)
	{
		free(instance->enabledErrorCheckerExtensions);
	}
	if (instance->enabledInstanceExtensions != NULL)
	{
		free(instance->enabledInstanceExtensions);
	}
	free(instance);
}
// END