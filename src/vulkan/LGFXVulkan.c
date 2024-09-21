#include "vulkan/LGFXVulkan.h"
#include "LGFXImpl.h"
#include "volk.h"
#include "Logging.h"
#include "memory.h"
#include "sync.h"
#include <string.h>

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
	// u32 propertiesCount = 0;
	// vkGetPhysicalDeviceQueueFamilyProperties(device, &propertiesCount, NULL);

	// VkQueueFamilyProperties *allQueueFamilyProperties = Allocate(VkQueueFamilyProperties, propertiesCount);
	// vkGetPhysicalDeviceQueueFamilyProperties(device, &propertiesCount, allQueueFamilyProperties);

	// for (u32 i = 0; i < propertiesCount; i++)
	// {
	// 	if (allQueueFamilyProperties[i].queueFlags & VK_PRESENT_GRAVITY_MAX_BIT_EXT)
	// 	{
	// 		free(allQueueFamilyProperties);
	// 		return false;
	// 	}
	// }
}

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