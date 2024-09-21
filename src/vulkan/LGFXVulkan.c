#include "vulkan/LGFXVulkan.h"
#include "LGFXImpl.h"
#include "volk.h"
#include "Logging.h"
#include "memory.h"
#include "sync.h"
#include <string.h>

// VULKAN-SPECIFIC HELPER STRUCTS
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
inline VkImageUsageFlags LGFXTextureUsage2Vulkan(LGFXTextureUsage usage)
{
	//these map 1:1
	return (VkImageUsageFlags)usage;
}
inline VkImageLayout LGFXTextureLayout2Vulkan(LGFXTextureLayout layout)
{
	switch (layout)
	{
		LGFXTextureLayout_Undefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		LGFXTextureLayout_General:
			return VK_IMAGE_LAYOUT_GENERAL;
		LGFXTextureLayout_ColorAttachmentOptimal:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		LGFXTextureLayout_DepthStencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		LGFXTextureLayout_DepthStencilReadOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		LGFXTextureLayout_ShaderReadOptimal:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		LGFXTextureLayout_TransferSrcOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		LGFXTextureLayout_TransferDstOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		LGFXTextureLayout_Preinitialized:
			return VK_IMAGE_LAYOUT_PREINITIALIZED;
		LGFXTextureLayout_PresentSource:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		LGFXTextureLayout_SharedPresentSrc:
			return VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
		LGFXTextureLayout_FragmentDensityMap:
			return VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
		LGFXTextureLayout_FragmentShadingRateAttachmentOptima:
			return VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
	}
}
VkFormat LGFXTextureFormat2Vulkan(LGFXTextureFormat format)
{
	switch (format)
	{
		LGFXTextureFormat_Undefined:
			return VK_FORMAT_UNDEFINED;
		LGFXTextureFormat_R8Unorm:
			return VK_FORMAT_R8_UNORM;
		LGFXTextureFormat_R8Snorm:
			return VK_FORMAT_R8_SNORM;
		LGFXTextureFormat_R8Uint:
			return VK_FORMAT_R8_UINT;
		LGFXTextureFormat_R8Sint:
			return VK_FORMAT_R8_SINT;
		LGFXTextureFormat_R16Uint:
			return VK_FORMAT_R16_UINT;
		LGFXTextureFormat_R16Sint:
			return VK_FORMAT_R16_SINT;
		LGFXTextureFormat_R16Float:
			return VK_FORMAT_R16_SFLOAT;
		LGFXTextureFormat_RG8Unorm:
			return VK_FORMAT_R8G8_UNORM;
		LGFXTextureFormat_RG8Snorm:
			return VK_FORMAT_R8G8_SNORM;
		LGFXTextureFormat_RG8Uint:
			return VK_FORMAT_R8G8_UINT;
		LGFXTextureFormat_RG8Sint:
			return VK_FORMAT_R8G8_SINT;
		LGFXTextureFormat_R32Float:
			return VK_FORMAT_R32_SFLOAT;
		LGFXTextureFormat_R32Uint:
			return VK_FORMAT_R32_UINT;
		LGFXTextureFormat_R32Sint:
			return VK_FORMAT_R32_SINT;
		LGFXTextureFormat_RG16Uint:
			return VK_FORMAT_R16G16_UINT;
		LGFXTextureFormat_RG16Sint:
			return VK_FORMAT_R16G16_SINT;
		LGFXTextureFormat_RG16Float:
			return VK_FORMAT_R16G16_SFLOAT;
		LGFXTextureFormat_RGBA8Unorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		LGFXTextureFormat_RGBA8UnormSrgb:
			return VK_FORMAT_R8G8B8A8_SRGB;
		LGFXTextureFormat_RGBA8Snorm:
			return VK_FORMAT_R8G8B8A8_SNORM;
		LGFXTextureFormat_RGBA8Uint:
			return VK_FORMAT_R8G8B8A8_UINT;
		LGFXTextureFormat_RGBA8Sint:
			return VK_FORMAT_R8G8B8A8_SINT;
		LGFXTextureFormat_BGRA8Unorm:
			return VK_FORMAT_B8G8R8A8_UNORM;
		LGFXTextureFormat_BGRA8UnormSrgb:
			return VK_FORMAT_B8G8R8A8_SRGB;
		LGFXTextureFormat_RGB10A2Uint:
			return VK_FORMAT_A2R10G10B10_UINT_PACK32;
		LGFXTextureFormat_RGB10A2Unorm:
			return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
		LGFXTextureFormat_RG11B10Ufloat:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		LGFXTextureFormat_RGB9E5Ufloat:
			return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		LGFXTextureFormat_RG32Float:
			return VK_FORMAT_R32G32_SFLOAT;
		LGFXTextureFormat_RG32Uint:
			return VK_FORMAT_R32G32_UINT;
		LGFXTextureFormat_RG32Sint:
			return VK_FORMAT_R32G32_SINT;
		LGFXTextureFormat_RGBA16Uint:
			return VK_FORMAT_R16G16B16A16_UINT;
		LGFXTextureFormat_RGBA16Sint:
			return VK_FORMAT_R16G16B16A16_SINT;
		LGFXTextureFormat_RGBA16Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		LGFXTextureFormat_RGBA32Float:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		LGFXTextureFormat_RGBA32Uint:
			return VK_FORMAT_R32G32B32A32_UINT;
		LGFXTextureFormat_RGBA32Sint:
			return VK_FORMAT_R32G32B32A32_SINT;
		LGFXTextureFormat_Stencil8:
			return VK_FORMAT_S8_UINT;
		LGFXTextureFormat_Depth16Unorm:
			return VK_FORMAT_D16_UNORM;
		LGFXTextureFormat_Depth24Plus:
			return VK_FORMAT_X8_D24_UNORM_PACK32;
		LGFXTextureFormat_Depth24PlusStencil8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		LGFXTextureFormat_Depth32Float:
			return VK_FORMAT_D32_SFLOAT;
		LGFXTextureFormat_Depth32FloatStencil8:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		LGFXTextureFormat_BC1RGBAUnorm:
			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		LGFXTextureFormat_BC1RGBAUnormSrgb:
			return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
		LGFXTextureFormat_BC2RGBAUnorm:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		LGFXTextureFormat_BC2RGBAUnormSrgb:
			return VK_FORMAT_BC2_SRGB_BLOCK;
		LGFXTextureFormat_BC3RGBAUnorm:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		LGFXTextureFormat_BC3RGBAUnormSrgb:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		LGFXTextureFormat_BC4RUnorm:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		LGFXTextureFormat_BC4RSnorm:
			return VK_FORMAT_BC4_SNORM_BLOCK;
		LGFXTextureFormat_BC5RGUnorm:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		LGFXTextureFormat_BC5RGSnorm:
			return VK_FORMAT_BC5_SNORM_BLOCK;
		LGFXTextureFormat_BC6HRGBUfloat:
			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		LGFXTextureFormat_BC6HRGBFloat:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		LGFXTextureFormat_BC7RGBAUnorm:
			return VK_FORMAT_BC7_SRGB_BLOCK;
		LGFXTextureFormat_BC7RGBAUnormSrgb:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		LGFXTextureFormat_ETC2RGB8Unorm:
			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
		LGFXTextureFormat_ETC2RGB8UnormSrgb:
			return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
		LGFXTextureFormat_ETC2RGB8A1Unorm:
			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
		LGFXTextureFormat_ETC2RGB8A1UnormSrg:
			return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
		LGFXTextureFormat_ETC2RGBA8Unorm:
			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		LGFXTextureFormat_ETC2RGBA8UnormSrgb:
			return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
		LGFXTextureFormat_EACR11Unorm:
			return VK_FORMAT_EAC_R11_UNORM_BLOCK;
		LGFXTextureFormat_EACR11Snorm:
			return VK_FORMAT_EAC_R11_SNORM_BLOCK;
		LGFXTextureFormat_EACRG11Unorm:
			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		LGFXTextureFormat_EACRG11Snorm:
			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		LGFXTextureFormat_ASTC4x4Unorm:
			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		LGFXTextureFormat_ASTC4x4UnormSrgb:
			return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
		LGFXTextureFormat_ASTC5x4Unorm:
			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
		LGFXTextureFormat_ASTC5x4UnormSrgb:
			return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
		LGFXTextureFormat_ASTC5x5Unorm:
			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
		LGFXTextureFormat_ASTC5x5UnormSrgb:
			return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
		LGFXTextureFormat_ASTC6x5Unorm:
			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
		LGFXTextureFormat_ASTC6x5UnormSrgb:
			return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
		LGFXTextureFormat_ASTC6x6Unorm:
			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
		LGFXTextureFormat_ASTC6x6UnormSrgb:
			return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
		LGFXTextureFormat_ASTC8x5Unorm:
			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
		LGFXTextureFormat_ASTC8x5UnormSrgb:
			return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
		LGFXTextureFormat_ASTC8x6Unorm:
			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
		LGFXTextureFormat_ASTC8x6UnormSrgb:
			return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
		LGFXTextureFormat_ASTC8x8Unorm:
			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
		LGFXTextureFormat_ASTC8x8UnormSrgb:
			return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
		LGFXTextureFormat_ASTC10x5Unorm:
			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
		LGFXTextureFormat_ASTC10x5UnormSrgb:
			return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
		LGFXTextureFormat_ASTC10x6Unorm:
			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
		LGFXTextureFormat_ASTC10x6UnormSrgb:
			return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
		LGFXTextureFormat_ASTC10x8Unorm:
			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
		LGFXTextureFormat_ASTC10x8UnormSrgb:
			return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
		LGFXTextureFormat_ASTC10x10Unorm:
			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
		LGFXTextureFormat_ASTC10x10UnormSrgb:
			return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
		LGFXTextureFormat_ASTC12x10Unorm:
			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
		LGFXTextureFormat_ASTC12x10UnormSrgb:
			return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
		LGFXTextureFormat_ASTC12x12Unorm:
			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
		LGFXTextureFormat_ASTC12x12UnormSrgb:
			return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
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
    for (int i = 0; i < supported->supportedSurfaceFormatsCount; i++)
    {
        if (supported->supportedSurfaceFormats[i].colorSpace == colorSpace && supported->supportedSurfaceFormats[i].format == format)
        {
            return supported->supportedSurfaceFormats[i];
        }
    }
	return supported->supportedSurfaceFormats[0];
}
// END

// HELPER FUNCTIONS
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
void VkLGFXDestroyFence(LGFXFence fence);

LGFXCommandQueue VkLGFXCreateCommandQueue(LGFXDevice device, VkQueue vkQueue)
{
	LGFXCommandQueueImpl *result = Allocate(LGFXCommandQueueImpl, 1);

	result->queue = vkQueue;
	result->queueMutex = Allocate(Mutex, 1);
	*result->queueMutex = NewMutex();
	result->fence = VkLGFXCreateFence(device, true);
	result->inDevice = device;

	return result;
}

void VkLGFXGetQueueCreateInfos(VkLGFXQueueProperties *properties, VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo *outInfos, u32 *outInfoCount, float *nilPriorityTodo)
{
    if (properties->dedicatedTransferQueueIndex != -1) return properties->dedicatedTransferQueueIndex;

	VkQueueFamilyProperties *all = (VkQueueFamilyProperties *)properties->allQueueInfos;

	i32 maxGraphicsScore = -100;
	i32 maxComputeScore = -100;
	i32 maxTransferScore = -100;
	for (usize i = 0; i < properties->allQueueInfosCount; i++)
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
            properties->dedicatedTransferQueueIndex = i;
        }
        if (computeScore > maxComputeScore)
        {
            maxComputeScore = computeScore;
            properties->dedicatedTransferQueueIndex = i;
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
		result->enabledInstanceExtensions = Allocate(const char *, 1);
		result->enabledInstanceExtensions[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		instanceInfo.enabledExtensionCount = 1;
		instanceInfo.ppEnabledExtensionNames = result->enabledInstanceExtensions;
		instanceInfo.ppEnabledExtensionNames = NULL;
	}
	else
	{
		instanceInfo.enabledExtensionCount = 1;
	}
	instanceInfo.flags = 0;

	if (info->runtimeErrorChecking)
	{
		result->enabledErrorCheckerExtensions = Allocate(const char *, 1);
		result->enabledErrorCheckerExtensions[0] = "VK_LAYER_KHRONOS_validation";
		//check validation layer support
		u32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);

		VkLayerProperties *layerProperties = Allocate(VkLayerProperties, 1);
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties);

		u32 totalSupported = 0;
		for (usize i = 0; i < layerCount; i++)
		{
			if (strcmp("VK_LAYER_KHRONOS_validation", layerProperties[i].layerName) == 0)
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
	const char *extensionNames = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

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

	VkPhysicalDeviceFeatures deviceEnabledFeatures = {0};
	//no easier way to do this... oh well
	deviceEnabledFeatures.multiDrawIndirect = info->requiredFeatures.multiDrawIndirect;
	deviceEnabledFeatures.depthClamp = info->requiredFeatures.depthClamp;
	deviceEnabledFeatures.fillModeNonSolid = info->requiredFeatures.fillModeNonSolid;
	deviceEnabledFeatures.wideLines = info->requiredFeatures.wideLines;
	deviceEnabledFeatures.textureCompressionETC2 = info->requiredFeatures.textureCompressionETC2;
	deviceEnabledFeatures.textureCompressionASTC_LDR = info->requiredFeatures.textureCompressionASTC;
	deviceEnabledFeatures.textureCompressionBC = info->requiredFeatures.textureCompressionBC;
	deviceEnabledFeatures.vertexPipelineStoresAndAtomics = info->requiredFeatures.vertexShaderCanStoreDataAndAtomics;
	deviceEnabledFeatures.fragmentStoresAndAtomics = info->requiredFeatures.fragmentShaderCanStoreDataAndAtomics;
	deviceEnabledFeatures.shaderFloat64 = info->requiredFeatures.shaderFloat64;
	deviceEnabledFeatures.shaderInt64 = info->requiredFeatures.shaderInt64;
	deviceEnabledFeatures.shaderInt16 = info->requiredFeatures.shaderInt16;
	deviceEnabledFeatures.sparseBinding = info->requiredFeatures.sparseBinding;
	deviceEnabledFeatures.shaderUniformBufferArrayDynamicIndexing = info->requiredFeatures.bindlessUniformBufferArrays;
	deviceEnabledFeatures.shaderSampledImageArrayDynamicIndexing = info->requiredFeatures.bindlessSamplerAndTextureArrays;
	deviceEnabledFeatures.shaderStorageBufferArrayDynamicIndexing = info->requiredFeatures.bindlessStorageBufferArrays;
	deviceEnabledFeatures.shaderStorageImageArrayDynamicIndexing = info->requiredFeatures.bindlessStorageTextureArrays;

	VkDeviceCreateInfo logicalDeviceInfo = {0};
	logicalDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceInfo.queueCreateInfoCount = finalQueueCreateInfoCount;
	logicalDeviceInfo.pQueueCreateInfos = queueCreateInfos;
	logicalDeviceInfo.enabledExtensionCount = 1; //require swapchain support only
	logicalDeviceInfo.ppEnabledExtensionNames = &extensionNames;
	logicalDeviceInfo.enabledLayerCount = 0; //these basically dont exist
	logicalDeviceInfo.ppEnabledLayerNames = NULL;
	logicalDeviceInfo.pNext = NULL;
	logicalDeviceInfo.pEnabledFeatures = &deviceEnabledFeatures;

	VkDevice logicalDevice;
	VkResult vkResult = vkCreateDevice(bestPhysicalDevice, &logicalDeviceInfo, NULL, &logicalDevice);
	free(queueCreateInfos);
	if (vkResult != VK_SUCCESS)
	{
		return NULL;
	}

	LGFXDeviceImpl *result = Allocate(LGFXDeviceImpl, 1);
	result->physicalDevice = bestPhysicalDevice;
	result->logicalDevice = logicalDevice;

	{
		VkQueue graphicsQueue;
		vkGetDeviceQueue(result->physicalDevice, inputQueueProps.dedicatedGraphicsQueueIndex, 0, &graphicsQueue);

		result->graphicsQueue = VkLGFXCreateCommandQueue(result, graphicsQueue);
	}
	if (inputQueueProps.dedicatedComputeQueueIndex != inputQueueProps.dedicatedGraphicsQueueIndex)
	{
		VkQueue computeQueue;
		vkGetDeviceQueue(result->physicalDevice, inputQueueProps.dedicatedComputeQueueIndex, 0, &computeQueue);

		result->computeQueue = VkLGFXCreateCommandQueue(result, computeQueue);
	}
	else
	{
		result->computeQueue = result->graphicsQueue;
	}

	if (inputQueueProps.dedicatedTransferQueueIndex != inputQueueProps.dedicatedGraphicsQueueIndex)
	{
		VkQueue transferQueue;
		vkGetDeviceQueue(result->physicalDevice, inputQueueProps.dedicatedComputeQueueIndex, 0, &transferQueue);

		result->transferQueue = VkLGFXCreateCommandQueue(result, transferQueue);
	}
	else
	{
		result->transferQueue = result->graphicsQueue;
	}

	return result;
}

LGFXSwapchain VkLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info)
{
	VkLGFXSwapchainSupport details = VkLGFXQuerySwapchainSupportDetails((VkPhysicalDevice)device->physicalDevice, (VkSurfaceKHR)info->windowSurface);
	VkSurfaceFormatKHR surfaceFormat = VkLGFXFindSurface(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_FORMAT_B8G8R8A8_UNORM, details.supportedSurfaceFormats);

	LGFXSwapchain result = Allocate(LGFXSwapchainImpl, 1);
	result->presentMode = info->presentationMode;
	result->swapchain = NULL;
	result->currentImageIndex = 0;
	result->width = info->width;
	result->height = info->height;
	result->windowSurface = info->windowSurface;
	result->device = device;

	vkDeviceWaitIdle((VkDevice)device->logicalDevice);

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = (VkSurfaceKHR)result->windowSurface;
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
    //createInfo.oldSwapchain = swapchain->handle;
	if (info->oldSwapchain != NULL)
	{
		createInfo.oldSwapchain = info->oldSwapchain->swapchain;
	}
	createInfo.imageExtent.width = result->width;
	createInfo.imageExtent.height = result->height;

    if (vkCreateSwapchainKHR((VkDevice)device->logicalDevice, &createInfo, NULL, &result->swapchain) != VK_SUCCESS)
    {
		VkLGFXDestroySwapcahin(result);
		return NULL;
    }

	if (vkGetSwapchainImagesKHR((VkDevice)device->logicalDevice, (VkSwapchainKHR)result->swapchain, &result->imageCount, NULL)!= VK_SUCCESS)
    {
		VkLGFXDestroySwapcahin(result);
		return NULL;
    }
	result->images = Allocate(void *, result->imageCount);
	vkGetSwapchainImagesKHR((VkDevice)device->logicalDevice, (VkSwapchainKHR)result->swapchain, &result->imageCount, (VkImage *)result->images);

	return result;
}
// END

// DESTROY FUNCTIONS
void VkLGFXDestroyFence(LGFXFence fence)
{
	vkDestroyFence((VkDevice)fence->device->logicalDevice, fence->fence, NULL);
}
void VkLGFXDestroyDevice(LGFXDevice device)
{
	if (device->logicalDevice != NULL)
	{
		vkDestroyDevice((VkDevice)device->logicalDevice, NULL);
	}
}
void VkLGFXDestroyInstance(LGFXInstance instance)
{
	if (instance->instance != NULL)
	{
		vkDestroyInstance((VkInstance)instance->instance, NULL);
		free(instance->instance);
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