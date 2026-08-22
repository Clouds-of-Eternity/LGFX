#include "webgpu/LGFXWebGPU.h"
#include "LGFXImpl.h"
#include "Logging.h"
#include "memory.h"
#include "lgfx/sync.h"
#include <string.h>
#include <math.h>
#include <limits.h>
#include "ArenaAllocator.h"

#ifndef __EMSCRIPTEN__
#error Attempted to compile LGFXWebGPU bindings for a platform other than the web
#endif

#include "webgpu/webgpu.h"
#include <emscripten/emscripten.h>

//Helper functions
static inline WGPUBufferUsage LGFXBufferUsage2WebGPU(LGFXBufferUsage usage)
{
    WGPUBufferUsage result = 0;
    if ((usage &LGFXBufferUsage_TransferSource) != 0)
    {
        result |= WGPUBufferUsage_CopySrc;
    }
    if ((usage &LGFXBufferUsage_TransferDest) != 0)
    {
        result |= WGPUBufferUsage_CopyDst;
    }
    if ((usage &LGFXBufferUsage_UniformBuffer) != 0)
    {
        result |= WGPUBufferUsage_Uniform;
    }
    if ((usage &LGFXBufferUsage_StorageBuffer) != 0)
    {
        result |= WGPUBufferUsage_Storage;
    }
    if ((usage &LGFXBufferUsage_IndexBuffer) != 0)
    {
        result |= WGPUBufferUsage_Index;
    }
    if ((usage &LGFXBufferUsage_VertexBuffer) != 0)
    {
        result |= WGPUBufferUsage_Vertex;
    }
    if ((usage &LGFXBufferUsage_IndirectDrawCallBuffer) != 0)
    {
        result |= WGPUBufferUsage_Indirect;
    }
    
    if ((usage & LGFXBufferUsage_UniformTexel) != 0 || (usage & LGFXBufferUsage_StorageTexel) != 0)
    {
        LGFX_WARN("Attempting to use LGFXBufferUsage_UniformTexel or LGFXBufferUsage_StorageTexel, which are unsupported on WebGPU\n");
    }
    return result;
}
static inline WGPUCompareFunction LGFXComparisonMode2WebGPU(LGFXComparisonMode mode)
{
    switch (mode)
    {
        case LGFXComparisonMode_Never:
        {
            return WGPUCompareFunction_Never;
        }
        case LGFXComparisonMode_Less:
        {
            return WGPUCompareFunction_Less;
        }
        case LGFXComparisonMode_Equal:
        {
            return WGPUCompareFunction_Equal;
        }
        case LGFXComparisonMode_LessEqual:
        {
            return WGPUCompareFunction_LessEqual;
        }
        case LGFXComparisonMode_Greater:
        {
            return WGPUCompareFunction_Greater;
        }
        case LGFXComparisonMode_NotEqual:
        {
            return WGPUCompareFunction_NotEqual;
        }
        case LGFXComparisonMode_GreaterEqual:
        {
            return WGPUCompareFunction_GreaterEqual;
        }
        case LGFXComparisonMode_Always:
        {
            return WGPUCompareFunction_Always;
        }
    }

}
static inline WGPUAddressMode LGFXSamplerRepeatMode2WebGPU(LGFXSamplerRepeatMode mode)
{
    if (mode == LGFXSamplerRepeatMode_Repeat)
    {
        return WGPUAddressMode_Repeat;
    }
    else
    {
        return WGPUAddressMode_ClampToEdge;
    }
}
static inline WGPUFilterMode LGFXSamplerFilterType2WebGPU(LGFXFilterType type)
{
    if (type == LGFXFilterType_Point)
    {
        return WGPUFilterMode_Nearest;
    }
    else 
    {
        return WGPUFilterMode_Linear;
    }
}
static inline WGPUPresentMode LGFXSwapchainPresentationMode2WebGPU(LGFXSwapchainPresentationMode mode)
{
    switch (mode)
    {
        case LGFXSwapchainPresentationMode_Fifo:
        {
            return WGPUPresentMode_Fifo;
        }
        case LGFXSwapchainPresentationMode_Immediate:
        {
            return WGPUPresentMode_Immediate;
        }
        case LGFXSwapchainPresentationMode_Mailbox:
        {
            return WGPUPresentMode_Mailbox;
        }
        default:
        {
            return WGPUPresentMode_Undefined;
        }
    }
}
static inline WGPUTextureUsage LGFXTextureUsage2WebGPU(LGFXTextureUsage value)
{
    WGPUTextureUsage result = 0;
    if ((value &LGFXTextureUsage_TransferSource) != 0)
    {
        result |= WGPUTextureUsage_CopySrc;
    }
    if ((value &LGFXTextureUsage_TransferDest) != 0)
    {
        result |= WGPUTextureUsage_CopyDst;
    }
    if ((value &LGFXTextureUsage_Sampled) != 0)
    {
        result |= WGPUTextureUsage_TextureBinding;
    }
    if ((value &LGFXTextureUsage_Storage) != 0)
    {
        result |= WGPUTextureUsage_StorageBinding;
    }
    if ((value &LGFXTextureUsage_ColorAttachment) != 0)
    {
        result |= WGPUTextureUsage_RenderAttachment;
    }
    if ((value &LGFXTextureUsage_DepthAttachment) != 0)
    {
        result |= WGPUTextureUsage_RenderAttachment;
    }
    if ((value &LGFXTextureUsage_TransientAttachment) != 0)
    {
        result |= WGPUTextureUsage_TransientAttachment;
    }
    if ((value &LGFXTextureUsage_InputAttachment) != 0)
    {
        result |= WGPUTextureUsage_RenderAttachment;
    }
    if ((value &LGFXTextureUsage_FragmentShadingRateAttachment) != 0 || (value &LGFXTextureUsage_FragmentDensityMap) != 0)
    {
        LGFX_WARN("Attempting to use LGFXTextureUsage_FragmentShadingRateAttachment or LGFXTextureUsage_FragmentDensityMap, which are unsupported on WebGPU\n");
    }
    return result;
}
static inline WGPUTextureFormat LGFXTextureFormat2WebGPU(LGFXTextureFormat format)
{
    switch (format)
    {
        case LGFXTextureFormat_Undefined:
        {
            return WGPUTextureFormat_Undefined;
        }
        case LGFXTextureFormat_R8Unorm:
        {
            return WGPUTextureFormat_R8Unorm;
        }
        case LGFXTextureFormat_R8Snorm:
        {
            return WGPUTextureFormat_R8Snorm;
        }
        case LGFXTextureFormat_R8Uint:
        {
            return WGPUTextureFormat_R8Uint;
        }
        case LGFXTextureFormat_R8Sint:
        {
            return WGPUTextureFormat_R8Sint;
        }
        case LGFXTextureFormat_R16Unorm:
        {
            return WGPUTextureFormat_R16Unorm;
        }
        case LGFXTextureFormat_R16Snorm:
        {
            return WGPUTextureFormat_R16Snorm;
        }
        case LGFXTextureFormat_R16Uint:
        {
            return WGPUTextureFormat_R16Uint;
        }
        case LGFXTextureFormat_R16Sint:
        {
            return WGPUTextureFormat_R16Sint;
        }
        case LGFXTextureFormat_R16Float:
        {
            return WGPUTextureFormat_R16Float;
        }
        case LGFXTextureFormat_RG8Unorm:
        {
            return WGPUTextureFormat_RG8Unorm;
        }
        case LGFXTextureFormat_RG8Snorm:
        {
            return WGPUTextureFormat_RG8Snorm;
        }
        case LGFXTextureFormat_RG8Uint:
        {
            return WGPUTextureFormat_RG8Uint;
        }
        case LGFXTextureFormat_RG8Sint:
        {
            return WGPUTextureFormat_RG8Sint;
        }
        case LGFXTextureFormat_R32Float:
        {
            return WGPUTextureFormat_R32Float;
        }
        case LGFXTextureFormat_R32Uint:
        {
            return WGPUTextureFormat_R32Uint;
        }
        case LGFXTextureFormat_R32Sint:
        {
            return WGPUTextureFormat_R32Sint;
        }
        case LGFXTextureFormat_RG16Uint:
        {
            return WGPUTextureFormat_RG16Uint;
        }
        case LGFXTextureFormat_RG16Sint:
        {
            return WGPUTextureFormat_RG16Sint;
        }
        case LGFXTextureFormat_RG16Float:
        {
            return WGPUTextureFormat_RG16Float;
        }
        case LGFXTextureFormat_RGBA8Unorm:
        {
            return WGPUTextureFormat_RGBA8Unorm;
        }
        case LGFXTextureFormat_RGBA8UnormSrgb:
        {
            return WGPUTextureFormat_RGBA8UnormSrgb;
        }
        case LGFXTextureFormat_RGBA8Snorm:
        {
            return WGPUTextureFormat_RGBA8Snorm;
        }
        case LGFXTextureFormat_RGBA8Uint:
        {
            return WGPUTextureFormat_RGBA8Uint;
        }
        case LGFXTextureFormat_RGBA8Sint:
        {
            return WGPUTextureFormat_RGBA8Sint;
        }
        case LGFXTextureFormat_BGRA8Unorm:
        {
            return WGPUTextureFormat_BGRA8Unorm;
        }
        case LGFXTextureFormat_BGRA8UnormSrgb:
        {
            return WGPUTextureFormat_BGRA8UnormSrgb;
        }
        case LGFXTextureFormat_RGB10A2Uint:
        {
            return WGPUTextureFormat_RGB10A2Uint;
        }
        case LGFXTextureFormat_RGB10A2Unorm:
        {
            return WGPUTextureFormat_RGB10A2Unorm;
        }
        case LGFXTextureFormat_RG11B10Ufloat:
        {
            return WGPUTextureFormat_RG11B10Ufloat;
        }
        case LGFXTextureFormat_RGB9E5Ufloat:
        {
            return WGPUTextureFormat_RGB9E5Ufloat;
        }
        case LGFXTextureFormat_RG32Float:
        {
            return WGPUTextureFormat_RG32Float;
        }
        case LGFXTextureFormat_RG32Uint:
        {
            return WGPUTextureFormat_RG32Uint;
        }
        case LGFXTextureFormat_RG32Sint:
        {
            return WGPUTextureFormat_RG32Sint;
        }
        case LGFXTextureFormat_RGBA16Uint:
        {
            return WGPUTextureFormat_RGBA16Uint;
        }
        case LGFXTextureFormat_RGBA16Sint:
        {
            return WGPUTextureFormat_RGBA16Sint;
        }
        case LGFXTextureFormat_RGBA16Float:
        {
            return WGPUTextureFormat_RGBA16Float;
        }
        case LGFXTextureFormat_RGBA32Float:
        {
            return WGPUTextureFormat_RGBA32Float;
        }
        case LGFXTextureFormat_RGBA32Uint:
        {
            return WGPUTextureFormat_RGBA32Uint;
        }
        case LGFXTextureFormat_RGBA32Sint:
        {
            return WGPUTextureFormat_RGBA32Sint;
        }
        case LGFXTextureFormat_BC1RGBAUnorm:
        {
            return WGPUTextureFormat_BC1RGBAUnorm;
        }
        case LGFXTextureFormat_BC1RGBAUnormSrgb:
        {
            return WGPUTextureFormat_BC1RGBAUnormSrgb;
        }
        case LGFXTextureFormat_BC2RGBAUnorm:
        {
            return WGPUTextureFormat_BC2RGBAUnorm;
        }
        case LGFXTextureFormat_BC2RGBAUnormSrgb:
        {
            return WGPUTextureFormat_BC2RGBAUnormSrgb;
        }
        case LGFXTextureFormat_BC3RGBAUnorm:
        {
            return WGPUTextureFormat_BC3RGBAUnorm;
        }
        case LGFXTextureFormat_BC3RGBAUnormSrgb:
        {
            return WGPUTextureFormat_BC3RGBAUnormSrgb;
        }
        case LGFXTextureFormat_BC4RUnorm:
        {
            return WGPUTextureFormat_BC4RUnorm;
        }
        case LGFXTextureFormat_BC4RSnorm:
        {
            return WGPUTextureFormat_BC4RSnorm;
        }
        case LGFXTextureFormat_BC5RGUnorm:
        {
            return WGPUTextureFormat_BC5RGUnorm;
        }
        case LGFXTextureFormat_BC5RGSnorm:
        {
            return WGPUTextureFormat_BC5RGSnorm;
        }
        case LGFXTextureFormat_BC6HRGBUfloat:
        {
            return WGPUTextureFormat_BC6HRGBUfloat;
        }
        case LGFXTextureFormat_BC6HRGBFloat:
        {
            return WGPUTextureFormat_BC6HRGBFloat;
        }
        case LGFXTextureFormat_BC7RGBAUnorm:
        {
            return WGPUTextureFormat_BC7RGBAUnorm;
        }
        case LGFXTextureFormat_BC7RGBAUnormSrgb:
        {
            return WGPUTextureFormat_BC7RGBAUnormSrgb;
        }
        case LGFXTextureFormat_ETC2RGB8Unorm:
        {
            return WGPUTextureFormat_ETC2RGB8Unorm;
        }
        case LGFXTextureFormat_ETC2RGB8UnormSrgb:
        {
            return WGPUTextureFormat_ETC2RGB8UnormSrgb;
        }
        case LGFXTextureFormat_ETC2RGB8A1Unorm:
        {
            return WGPUTextureFormat_ETC2RGB8A1Unorm;
        }
        case LGFXTextureFormat_ETC2RGB8A1UnormSrgb:
        {
            return WGPUTextureFormat_ETC2RGB8A1UnormSrgb;
        }
        case LGFXTextureFormat_ETC2RGBA8Unorm:
        {
            return WGPUTextureFormat_ETC2RGBA8Unorm;
        }
        case LGFXTextureFormat_ETC2RGBA8UnormSrgb:
        {
            return WGPUTextureFormat_ETC2RGBA8UnormSrgb;
        }
        case LGFXTextureFormat_EACR11Unorm:
        {
            return WGPUTextureFormat_EACR11Unorm;
        }
        case LGFXTextureFormat_EACR11Snorm:
        {
            return WGPUTextureFormat_EACR11Snorm;
        }
        case LGFXTextureFormat_EACRG11Unorm:
        {
            return WGPUTextureFormat_EACRG11Unorm;
        }
        case LGFXTextureFormat_EACRG11Snorm:
        {
            return WGPUTextureFormat_EACRG11Snorm;
        }
        case LGFXTextureFormat_ASTC4x4Unorm:
        {
            return WGPUTextureFormat_ASTC4x4Unorm;
        }
        case LGFXTextureFormat_ASTC4x4UnormSrgb:
        {
            return WGPUTextureFormat_ASTC4x4UnormSrgb;
        }
        case LGFXTextureFormat_ASTC5x4Unorm:
        {
            return WGPUTextureFormat_ASTC5x4Unorm;
        }
        case LGFXTextureFormat_ASTC5x4UnormSrgb:
        {
            return WGPUTextureFormat_ASTC5x4UnormSrgb;
        }
        case LGFXTextureFormat_ASTC5x5Unorm:
        {
            return WGPUTextureFormat_ASTC5x5Unorm;
        }
        case LGFXTextureFormat_ASTC5x5UnormSrgb:
        {
            return WGPUTextureFormat_ASTC5x5UnormSrgb;
        }
        case LGFXTextureFormat_ASTC6x5Unorm:
        {
            return WGPUTextureFormat_ASTC6x5Unorm;
        }
        case LGFXTextureFormat_ASTC6x5UnormSrgb:
        {
            return WGPUTextureFormat_ASTC6x5UnormSrgb;
        }
        case LGFXTextureFormat_ASTC6x6Unorm:
        {
            return WGPUTextureFormat_ASTC6x6Unorm;
        }
        case LGFXTextureFormat_ASTC6x6UnormSrgb:
        {
            return WGPUTextureFormat_ASTC6x6UnormSrgb;
        }
        case LGFXTextureFormat_ASTC8x5Unorm:
        {
            return WGPUTextureFormat_ASTC8x5Unorm;
        }
        case LGFXTextureFormat_ASTC8x5UnormSrgb:
        {
            return WGPUTextureFormat_ASTC8x5UnormSrgb;
        }
        case LGFXTextureFormat_ASTC8x6Unorm:
        {
            return WGPUTextureFormat_ASTC8x6Unorm;
        }
        case LGFXTextureFormat_ASTC8x6UnormSrgb:
        {
            return WGPUTextureFormat_ASTC8x6UnormSrgb;
        }
        case LGFXTextureFormat_ASTC8x8Unorm:
        {
            return WGPUTextureFormat_ASTC8x8Unorm;
        }
        case LGFXTextureFormat_ASTC8x8UnormSrgb:
        {
            return WGPUTextureFormat_ASTC8x8UnormSrgb;
        }
        case LGFXTextureFormat_ASTC10x5Unorm:
        {
            return WGPUTextureFormat_ASTC10x5Unorm;
        }
        case LGFXTextureFormat_ASTC10x5UnormSrgb:
        {
            return WGPUTextureFormat_ASTC10x5UnormSrgb;
        }
        case LGFXTextureFormat_ASTC10x6Unorm:
        {
            return WGPUTextureFormat_ASTC10x6Unorm;
        }
        case LGFXTextureFormat_ASTC10x6UnormSrgb:
        {
            return WGPUTextureFormat_ASTC10x6UnormSrgb;
        }
        case LGFXTextureFormat_ASTC10x8Unorm:
        {
            return WGPUTextureFormat_ASTC10x8Unorm;
        }
        case LGFXTextureFormat_ASTC10x8UnormSrgb:
        {
            return WGPUTextureFormat_ASTC10x8UnormSrgb;
        }
        case LGFXTextureFormat_ASTC10x10Unorm:
        {
            return WGPUTextureFormat_ASTC10x10Unorm;
        }
        case LGFXTextureFormat_ASTC10x10UnormSrgb:
        {
            return WGPUTextureFormat_ASTC10x10UnormSrgb;
        }
        case LGFXTextureFormat_ASTC12x10Unorm:
        {
            return WGPUTextureFormat_ASTC12x10Unorm;
        }
        case LGFXTextureFormat_ASTC12x10UnormSrgb:
        {
            return WGPUTextureFormat_ASTC12x10UnormSrgb;
        }
        case LGFXTextureFormat_ASTC12x12Unorm:
        {
            return WGPUTextureFormat_ASTC12x12Unorm;
        }
        case LGFXTextureFormat_ASTC12x12UnormSrgb:
        {
            return WGPUTextureFormat_ASTC12x12UnormSrgb;
        }
        case LGFXTextureFormat_Stencil8:
        {
            return WGPUTextureFormat_Stencil8;
        }
        case LGFXTextureFormat_Depth16Unorm:
        {
            return WGPUTextureFormat_Depth16Unorm;
        }
        case LGFXTextureFormat_Depth24Plus:
        {
            return WGPUTextureFormat_Depth24Plus;
        }
        case LGFXTextureFormat_Depth24PlusStencil8:
        {
            return WGPUTextureFormat_Depth24PlusStencil8;
        }
        case LGFXTextureFormat_Depth32Float:
        {
            return WGPUTextureFormat_Depth32Float;
        }
        case LGFXTextureFormat_Depth32FloatStencil8:
        {
            return WGPUTextureFormat_Depth32FloatStencil8;
        }
    }
}
static inline void wRequestAdapterCallback(WGPURequestAdapterStatus status, WGPUAdapter potentialAdapter, WGPUStringView message, void *userdata1, void *userdata2)
{
    WGPUAdapter *adapter = (WGPUAdapter *)userdata1;
    if (status != WGPURequestAdapterStatus_Success)
    {
        printf("%s\n", message.data);
        exit(0);
    }
    *adapter = potentialAdapter;
}
static inline void wRequestDeviceCallback(WGPURequestDeviceStatus status, WGPUDevice potentialDevice, WGPUStringView message, WGPU_NULLABLE void* userdata1, WGPU_NULLABLE void* userdata2)
{
    WGPUDevice *device = (WGPUDevice *)userdata1;
    if (status != WGPURequestDeviceStatus_Success)
    {
        printf("%s\n", message.data);
        exit(0);
    }
    *device = potentialDevice;
}
static inline void wLGFXAwaitFuture(WGPUFuture future, LGFXInstance instance)
{
    WGPUFutureWaitInfo futureWaitInfo = {0};
    futureWaitInfo.future = future;
    futureWaitInfo.completed = false;
    wgpuInstanceWaitAny((WGPUInstance)instance->instance, 1, &futureWaitInfo, (uint64_t)0xFFFFFFFFFFFFFFFF);
}

LGFXInstance wLGFXCreateInstance(LGFXInstanceCreateInfo *info)
{
	LGFXInstance result = Allocate(LGFXInstanceImpl, 1);
    *result = (LGFXInstanceImpl){0};
    result->runtimeErrorChecking = info->runtimeErrorChecking;
    result->backend = info->backend;

    const WGPUInstanceFeatureName kTimedWaitAny = WGPUInstanceFeatureName_TimedWaitAny;
    WGPUInstanceDescriptor instanceDescriptor = {0};
    instanceDescriptor.nextInChain = NULL;
    instanceDescriptor.requiredFeatureCount = 1;
    instanceDescriptor.requiredFeatures = &kTimedWaitAny;

    WGPUInstance instance = wgpuCreateInstance(&instanceDescriptor);
    result->instance = instance;
    result->enabledErrorCheckerExtensions = NULL;

}
LGFXDevice wLGFXCreateDevice(LGFXInstance instance, LGFXDeviceCreateInfo *info)
{
    //obtain adapter (serves as the 'physical' device in this case)
    WGPUAdapter adapter = NULL;
    WGPURequestAdapterCallbackInfo requestAdapterCallbackInfo = {0};
    requestAdapterCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    requestAdapterCallbackInfo.callback = wRequestAdapterCallback;
    requestAdapterCallbackInfo.userdata1 = &adapter;

    wLGFXAwaitFuture(wgpuInstanceRequestAdapter(instance, NULL, requestAdapterCallbackInfo), instance);

    LGFXDevice result = Allocate(LGFXDeviceImpl, 1);
    *result = (LGFXDeviceImpl){0};
    result->physicalDevice = adapter;

	WGPUInstance wgpuInstance = (WGPUInstance)instance->instance;

    WGPUDevice device = NULL;
    WGPURequestDeviceCallbackInfo requestDeviceCallbackInfo = {0};
    requestDeviceCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    requestDeviceCallbackInfo.callback = &wRequestDeviceCallback;
    requestDeviceCallbackInfo.userdata1 = &device;

    wLGFXAwaitFuture(wgpuAdapterRequestDevice(adapter, NULL, requestDeviceCallbackInfo), instance);

    //webgpu only allows a single queue for now
    result->instance = instance;
    result->backend = LGFXBackendType_WebGPU;
    result->fencePool = (LGFXFencePool){0};
    result->logicalDevice = device;
    result->graphicsQueue = Allocate(LGFXCommandQueueImpl, 1);
    *result->graphicsQueue = (LGFXCommandQueueImpl){0};
    result->graphicsQueue->inDevice = result;
    result->graphicsQueue->queueLock = NewLock();
    result->graphicsQueue->queue = wgpuDeviceGetQueue(device);
    result->computeQueue = NULL;
    result->transferQueue = NULL;
}

//webgpu does not have explicit sync
LGFXFence wLGFXCreateFence(LGFXDevice device, bool signalled)
{
    return NULL;
}
void wLGFXAwaitFence(LGFXFence fence)
{

}
void wLGFXResetFence(LGFXFence fence)
{

}
void wLGFXDestroyFence(LGFXFence fence)
{
}

LGFXSemaphore wLGFXCreateSemaphore(LGFXDevice device)
{
    return NULL;
}
void wLGFXDestroySemaphore(LGFXSemaphore semaphore)
{

}

void wLGFXAwaitWriteFunction(LGFXCommandBuffer commandBuffer, LGFXFunctionType funcType, LGFXFunctionOperationType opType)
{

}
void wLGFXAwaitDraw(LGFXCommandBuffer commandBuffer)
{

}
void wLGFXAwaitGraphicsIdle(LGFXDevice device)
{

}

LGFXSwapchain wLGFXCreateSwapchain(LGFXDevice device, LGFXSwapchainCreateInfo *info)
{
    WGPUSurface surface;
	if (info->oldSwapchain == NULL)
	{
		int32_t createSurfaceResult = info->createSurfaceFunc(device->instance->instance, info->windowHandle, NULL, (void**)&surface);
    }
    else
    {
        surface = (WGPUSurface)info->oldSwapchain->windowSurface;
    }

    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;
    WGPUAdapter adapter = (WGPUAdapter)device->physicalDevice;

	LGFXSwapchain result = Allocate(LGFXSwapchainImpl, 1);
    *result = (LGFXSwapchainImpl){0};
	result->swapchain = NULL;
	result->currentImageIndex = 0;
	result->currentFrameIndex = 0;
	result->width = info->width;
	result->height = info->height;
	//result->nativeWindowHandle = info->nativeWindowHandle;
	result->windowSurface = surface;
	result->device = device;
	result->justCreated = true;
	result->invalidated = false;
	result->createSurfaceFunc = info->createSurfaceFunc;
	result->windowHandle = info->windowHandle;

    WGPUSurfaceCapabilities surfaceCapabilities = {};
    wgpuSurfaceGetCapabilities(surface, adapter, &surfaceCapabilities);

    WGPUSurfaceConfiguration config = {};
    config.device = device;
    config.format = surfaceCapabilities.formats[0];
    config.usage = WGPUTextureUsage_RenderAttachment;
    config.width = info->width;
    config.height = info->height;
    config.presentMode = LGFXSwapchainPresentationMode2WebGPU(info->presentationMode);

    bool hasPresentMode = false;
    for (uint32_t i = 0; i < surfaceCapabilities.presentModeCount; i++)
    {
        if (config.presentMode == surfaceCapabilities.presentModes[i])
        {
            hasPresentMode = true;
            break;
        }
    }
    if (!hasPresentMode)
    {
        LGFXSwapchainPresentationMode overridePresentMode;

        config.presentMode = surfaceCapabilities.presentModes[0];
        switch (config.presentMode)
        {
            case WGPUPresentMode_FifoRelaxed:
            case WGPUPresentMode_Fifo:
            {
                overridePresentMode = LGFXSwapchainPresentationMode_Fifo;
                break;
            }
            case WGPUPresentMode_Immediate:
            {
                overridePresentMode = LGFXSwapchainPresentationMode_Immediate;
                break;
            }
            case WGPUPresentMode_Mailbox:
            {
                overridePresentMode = LGFXSwapchainPresentationMode_Mailbox;
                break;
            }
            default:
            {
                printf("Fatal error: No known presentation modes available for WebGPU");
                abort();
                break;
            }
        }
        LGFX_WARN("User-specified LGFXSwapchainPresentationMode with ID %u not supported, falling back to WGPUPresentMode of value %u instead.\n", info->presentationMode, config.presentMode);
        result->presentMode = overridePresentMode;
    }
	else result->presentMode = info->presentationMode;

    result->backbufferTexturesCount = 1;
    result->currentFrameIndex = 0;
    result->currentImageIndex = 0;

    result->frameDatas = Allocate(LGFXSwapchainFrame, 1);
    result->frameDatas[0] = (LGFXSwapchainFrame){0};

    LGFXTexture backbufferTexture = Allocate(LGFXTextureImpl, 1);
    *backbufferTexture = (LGFXTextureImpl){0};
    backbufferTexture->ownsHandle = false;
    backbufferTexture->device = device;
    backbufferTexture->depth = 1;
    backbufferTexture->width = info->width;
    backbufferTexture->height = info->height;
    backbufferTexture->imageHandle = NULL; //to be set later
    backbufferTexture->imageView = NULL;
    backbufferTexture->layout = LGFXTextureLayout_General; //webgpu doesn't give control over texture layouts
    backbufferTexture->format = config.format;
    backbufferTexture->mipLevels = 1;
    backbufferTexture->sampleCount = 1;
    result->frameDatas[0].backbufferTexture = backbufferTexture;

    LGFXTexture backDepthbufferTexture = Allocate(LGFXTextureImpl, 1);
    *backDepthbufferTexture = (LGFXTextureImpl){0};
    backDepthbufferTexture->ownsHandle = true;
    backDepthbufferTexture->depth = 1;
    backDepthbufferTexture->width = info->width;
    backDepthbufferTexture->height = info->height;
    backDepthbufferTexture->layout = LGFXTextureLayout_DepthStencilAttachmentOptimal; //webgpu doesn't give control over texture layouts
    backDepthbufferTexture->format = LGFXTextureFormat_Depth32Float;
    backDepthbufferTexture->mipLevels = 1;
    backDepthbufferTexture->sampleCount = 1;

    WGPUTextureDescriptor depthTextureDescriptor = (WGPUTextureDescriptor){0};
    depthTextureDescriptor.dimension = WGPUTextureDimension_2D;
    depthTextureDescriptor.format = WGPUTextureFormat_Depth32Float;
    depthTextureDescriptor.label.data = "LGFX Backbuffer Depth Data";
    depthTextureDescriptor.label.length = WGPU_STRLEN;
    depthTextureDescriptor.mipLevelCount = 1;
    depthTextureDescriptor.sampleCount = 1;
    depthTextureDescriptor.size.depthOrArrayLayers = 1;
    depthTextureDescriptor.size.width = info->width;
    depthTextureDescriptor.size.height = info->height;
    depthTextureDescriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc;

    backDepthbufferTexture->imageHandle = wgpuDeviceCreateTexture(wDevice, &depthTextureDescriptor); //can be set now
    backDepthbufferTexture->imageView = wgpuTextureCreateView(backDepthbufferTexture->imageHandle, NULL);

    result->frameDatas[0].backDepthbuffer = backDepthbufferTexture;

    wgpuSurfaceConfigure(surface, &config);
    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);

    return result;
}
void wLGFXDestroySwapchain(LGFXSwapchain swapchain, bool windowIsDestroyed)
{
	if (swapchain->swapchain != NULL)
	{
		for (uint32_t i = 0; i < swapchain->backbufferTexturesCount; i++)
		{
			LGFXDestroyTexture(swapchain->frameDatas[i].backbufferTexture);
			LGFXDestroyTexture(swapchain->frameDatas[i].backDepthbuffer);
		}
		//vkDestroySwapchainKHR((VkDevice)swapchain->device->logicalDevice, (VkSwapchainKHR)swapchain->swapchain, NULL);
	}
    wgpuSurfaceRelease((WGPUSurface)swapchain->windowSurface);
    free(swapchain);
}
LGFXSemaphore wLGFXSwapchainGetAwaitRenderedSemaphore(LGFXSwapchain swapchain)
{
    return NULL;
}
LGFXSemaphore wLGFXSwapchainGetAwaitPresentedSemaphore(LGFXSwapchain swapchain)
{
    return NULL;
}
uint32_t wLGFXSwapchainGetCurrentFrameIndex(LGFXSwapchain swapchain)
{
    return 0;
}
uint32_t wLGFXSwapchainGetCurrentImageIndex(LGFXSwapchain swapchain)
{
    return 0;
}

bool wLGFXNewFrame(LGFXDevice device, LGFXSwapchain *swapchain, uint32_t frameWidth, uint32_t frameHeight)
{
    LGFXSwapchain currentSwapchain = *swapchain;
    if (currentSwapchain != NULL)
    {
		if (!currentSwapchain->invalidated)
		{
            WGPUSurfaceTexture surfaceTexture = {};
            wgpuSurfaceGetCurrentTexture((WGPUSurface)currentSwapchain->windowSurface, &surfaceTexture);
        
            if (surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal || surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
            {
                currentSwapchain->frameDatas[0].backbufferTexture->imageHandle = surfaceTexture.texture;
                //Webgpu demands that the texture view be created then released every frame
                currentSwapchain->frameDatas[0].backbufferTexture->imageView = wgpuTextureCreateView(surfaceTexture.texture, NULL);
            }
            else if (surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Outdated || surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Timeout || surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Lost)
            {
                LGFXSwapchainCreateInfo createInfo = {0};
                createInfo.width = frameWidth;
                createInfo.height = frameHeight;
                createInfo.oldSwapchain = currentSwapchain;
                createInfo.presentationMode = currentSwapchain->presentMode;
                createInfo.windowHandle = currentSwapchain->windowHandle;
                createInfo.createSurfaceFunc = currentSwapchain->createSurfaceFunc;

                *swapchain = LGFXCreateSwapchain(currentSwapchain->device, &createInfo);
                printf("Swapchain recreated successfully\n");
                LGFXDestroySwapchain(currentSwapchain, false);
                printf("Old swapchain disposed\n");
            }
            else
            {
                //un-recoverable error
                printf("Fatal error: wgpuSurfaceGetCurrentTexture() failed with result of ID %u\n", surfaceTexture.status);
                abort();
            }
        }
    }
}
void wLGFXSubmitFrame(LGFXDevice device, LGFXSwapchain swapchain)
{
    //present only valid for native deployments
    //wgpuSurfacePresent((WGPUSurface)swapchain->windowSurface);
    wgpuTextureRelease((WGPUTexture)swapchain->frameDatas[0].backbufferTexture->imageHandle);
    wgpuTextureViewRelease((WGPUTextureView)swapchain->frameDatas[0].backbufferTexture->imageView);

	if (swapchain->justCreated)
	{
		swapchain->justCreated = false;
	}
}

void wLGFXDestroyInstance(LGFXInstance instance)
{
    wgpuInstanceRelease((WGPUInstance)instance->instance);
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
void wLGFXDestroyDevice(LGFXDevice device)
{
    if (device->graphicsQueue != NULL)
    {
        DestroyLock(&device->graphicsQueue->queueLock);
        wgpuQueueRelease((WGPUQueue)device->graphicsQueue->queue);
        free(device->graphicsQueue);
    }
    wgpuAdapterRelease((WGPUAdapter)device->physicalDevice);
    wgpuDeviceRelease((WGPUDevice)device->logicalDevice);

    free(device);
}

LGFXTexture wLGFXCreateTexture(LGFXDevice device, LGFXTextureCreateInfo *info)
{
    LGFXTexture result = Allocate(LGFXTextureImpl, 1);
    *result = (LGFXTextureImpl){0};
    result->depth = info->depth == 0 ? 1 : info->depth;
    result->device = device;
    result->format = info->format;
    result->width = info->width;
    result->height = info->height;
    result->layout = LGFXTextureLayout_General;
    result->mipLevels = info->mipLevels;
    result->ownsHandle = true;
    result->sampleCount = info->sampleCount;
    result->textureMemory = NULL;

    //create handle
    WGPUTextureDescriptor textureDescriptor = (WGPUTextureDescriptor){0};
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    if (info->depth > 1)
    {
        textureDescriptor.dimension = WGPUTextureDimension_3D;
    }
    textureDescriptor.format = LGFXTextureFormat2WebGPU(info->format);
    textureDescriptor.size.width = result->width;
    textureDescriptor.size.height = result->height;
    textureDescriptor.size.depthOrArrayLayers = result->depth;
    textureDescriptor.label.data = info->memoryIdentifierName;
    textureDescriptor.label.length = WGPU_STRLEN;
    textureDescriptor.mipLevelCount = result->mipLevels;
    textureDescriptor.sampleCount = result->sampleCount;
    textureDescriptor.usage = LGFXTextureUsage2WebGPU(info->usage);

    result->imageHandle = wgpuDeviceCreateTexture((WGPUDevice)device->logicalDevice, &textureDescriptor);
    //create view
    result->imageView = wgpuTextureCreateView((WGPUTexture)result->imageHandle, NULL);
}
void wLGFXTextureTransitionLayout(LGFXDevice device, LGFXTexture texture, LGFXTextureLayout targetLayout, LGFXCommandBuffer commandBuffer, uint32_t mipToTransition, uint32_t mipTransitionDepth)
{
    //doesnt do anything
    texture->layout = targetLayout;
}
void wLGFXTextureSetData(LGFXDevice device, LGFXTexture texture, uint8_t* bytes, size_t length)
{
    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;
    WGPUTexelCopyTextureInfo destInfo = (WGPUTexelCopyTextureInfo){0};
    destInfo.aspect = WGPUTextureAspect_All;
    destInfo.mipLevel = 1;
    destInfo.texture = (WGPUTexture)texture->imageHandle;

    WGPUTexelCopyBufferLayout srcInfo = (WGPUTexelCopyBufferLayout){0};
    srcInfo.bytesPerRow = texture->width * LGFXGetPixelSize(texture->format);
    srcInfo.rowsPerImage = texture->height;
    srcInfo.offset = 0;

    WGPUExtent3D writeSize = (WGPUExtent3D){0};
    writeSize.width = texture->width;
    writeSize.height = texture->height;
    writeSize.depthOrArrayLayers = texture->depth;

    wgpuQueueWriteTexture((WGPUQueue)device->graphicsQueue->queue, &destInfo, bytes, length, &srcInfo, &writeSize);
}
void wLGFXCopyBufferToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, uint32_t toMip)
{
    LGFXPoint3 extents;
    extents.X = to->width;
    extents.Y = to->height;
    extents.Z = to->depth;

    LGFXPoint3 offset = (LGFXPoint3){0};

    wLGFXCopyBufferToTextureWithExtents(device, commandBuffer, from, to, fromBufferOffset, extents, offset, toMip);
}
void wLGFXCopyBufferToTextureWithExtents(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXTexture to, size_t fromBufferOffset, LGFXPoint3 extents, LGFXPoint3 offset, uint32_t toMip)
{
    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;

    WGPUCommandEncoder encoder;
    
    if (commandBuffer != NULL)
    {
        encoder = (WGPUCommandEncoder)commandBuffer->cmdBuffer;
    }
    else
    {
        encoder = wgpuDeviceCreateCommandEncoder(wDevice, NULL);
    }

    WGPUTexelCopyBufferInfo srcInfo = (WGPUTexelCopyBufferInfo){0};
    srcInfo.buffer = (WGPUBuffer)from->handle;
    srcInfo.layout = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
    srcInfo.layout.offset = fromBufferOffset;

    WGPUTexelCopyTextureInfo destInfo = (WGPUTexelCopyTextureInfo){0};
    destInfo.aspect = WGPUTextureAspect_All;
    destInfo.origin.x = offset.X;
    destInfo.origin.y = offset.Y;
    destInfo.origin.z = offset.Z;
    destInfo.mipLevel = toMip;
    destInfo.texture = (WGPUTexture)to->imageHandle;

    WGPUExtent3D copySizeInfo;
    copySizeInfo.depthOrArrayLayers = extents.Z;
    copySizeInfo.width = extents.X;
    copySizeInfo.height = extents.Y;
    wgpuCommandEncoderCopyBufferToTexture(encoder, &srcInfo, &destInfo, &copySizeInfo);
    
    if (commandBuffer == NULL)
    {
        WGPUCommandBuffer finalizedCommands = wgpuCommandEncoderFinish(encoder, NULL);
        wgpuQueueSubmit((WGPUQueue)device->graphicsQueue->queue, 1, &finalizedCommands);
        wgpuCommandEncoderRelease(encoder);
        wgpuCommandBufferRelease(finalizedCommands);
    }
}
void wLGFXCopyTextureToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXBuffer to, uint32_t toMip)
{
    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;
    WGPUCommandEncoder encoder;
    
    if (commandBuffer != NULL)
    {
        encoder = (WGPUCommandEncoder)commandBuffer->cmdBuffer;
    }
    else
    {
        encoder = wgpuDeviceCreateCommandEncoder(wDevice, NULL);
    }
    
    WGPUTexelCopyTextureInfo srcInfo = (WGPUTexelCopyTextureInfo){0};
    srcInfo.aspect = WGPUTextureAspect_All;
    srcInfo.mipLevel = toMip;
    srcInfo.texture = (WGPUTexture)from->imageHandle;

    WGPUTexelCopyBufferInfo destInfo = (WGPUTexelCopyBufferInfo){0};
    destInfo.buffer = (WGPUBuffer)to->handle;
    destInfo.layout.bytesPerRow = LGFXGetPixelSize(from->format) * from->width;
    destInfo.layout.offset = 0;
    destInfo.layout.rowsPerImage = from->height;

    WGPUExtent3D copySizeInfo;
    copySizeInfo.depthOrArrayLayers = from->depth;
    copySizeInfo.width = from->width;
    copySizeInfo.height = from->height;

    wgpuCommandEncoderCopyTextureToBuffer(encoder, &srcInfo, &destInfo, &copySizeInfo);

    if (commandBuffer == NULL)
    {
        WGPUCommandBuffer finalizedCommands = wgpuCommandEncoderFinish(encoder, NULL);
        wgpuQueueSubmit((WGPUQueue)device->graphicsQueue->queue, 1, &finalizedCommands);
        wgpuCommandEncoderRelease(encoder);
        wgpuCommandBufferRelease(finalizedCommands);
    }
}
void wLGFXCopyTextureToTexture(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXTexture from, LGFXTexture to, LGFXPoint3 fromOffset, uint32_t fromMip, LGFXPoint3 toOffset, uint32_t toMip, LGFXPoint3 copyAreaSize, bool autoTransition)
{
    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;
    WGPUCommandEncoder encoder;
    
    if (commandBuffer != NULL)
    {
        encoder = (WGPUCommandEncoder)commandBuffer->cmdBuffer;
    }
    else
    {
        encoder = wgpuDeviceCreateCommandEncoder(wDevice, NULL);
    }
    
    WGPUTexelCopyTextureInfo srcInfo = (WGPUTexelCopyTextureInfo){0};
    srcInfo.aspect = WGPUTextureAspect_All;
    srcInfo.mipLevel = fromMip;
    srcInfo.origin.x = fromOffset.X;
    srcInfo.origin.y = fromOffset.Y;
    srcInfo.origin.z = fromOffset.Z;
    srcInfo.texture = (WGPUTexture)from->imageHandle;

    WGPUTexelCopyTextureInfo destInfo = (WGPUTexelCopyTextureInfo){0};
    destInfo.aspect = WGPUTextureAspect_All;
    destInfo.mipLevel = toMip;
    destInfo.origin.x = toOffset.X;
    destInfo.origin.y = toOffset.Y;
    destInfo.origin.z = toOffset.Z;
    destInfo.texture = (WGPUTexture)to->imageHandle;

    WGPUExtent3D copySize;
    copySize.width = copyAreaSize.X;
    copySize.height = copyAreaSize.Y;
    copySize.depthOrArrayLayers = copyAreaSize.Z;

    wgpuCommandEncoderCopyTextureToTexture(encoder, &srcInfo, &destInfo, &copySize);

    if (commandBuffer == NULL)
    {
        WGPUCommandBuffer finalizedCommands = wgpuCommandEncoderFinish(encoder, NULL);
        wgpuQueueSubmit((WGPUQueue)device->graphicsQueue->queue, 1, &finalizedCommands);
        wgpuCommandEncoderRelease(encoder);
        wgpuCommandBufferRelease(finalizedCommands);
    }
}
void wLGFXDestroyTexture(LGFXTexture texture)
{
    wgpuTextureViewRelease((WGPUTextureView)texture->imageView);
    if (texture->ownsHandle)
    {
        wgpuTextureRelease((WGPUTexture)texture->imageHandle);
    }
    free(texture);
}

LGFXSamplerState wLGFXCreateSamplerState(LGFXDevice device, LGFXSamplerStateCreateInfo *info)
{
    WGPUSamplerDescriptor samplerDescriptor = (WGPUSamplerDescriptor){0};
    samplerDescriptor.addressModeU = LGFXSamplerRepeatMode2WebGPU(info->repeatModeU);
    samplerDescriptor.addressModeV = LGFXSamplerRepeatMode2WebGPU(info->repeatModeV);
    samplerDescriptor.addressModeW = LGFXSamplerRepeatMode2WebGPU(info->repeatModeW);
    samplerDescriptor.compare = LGFXComparisonMode2WebGPU(info->comparisonOperation);
    samplerDescriptor.lodMinClamp = info->minLODClamp;
    samplerDescriptor.lodMaxClamp = info->maxLODClamp;
    samplerDescriptor.maxAnisotropy = info->isAnisotropic ? info->maxAnisotropy : 0.0f;
    samplerDescriptor.mipmapFilter = LGFXSamplerFilterType2WebGPU(info->mipmapLookupMode);
    samplerDescriptor.minFilter = LGFXSamplerFilterType2WebGPU(info->minimizationFilter);
    samplerDescriptor.magFilter = LGFXSamplerFilterType2WebGPU(info->magnificationFilter);

    LGFXSamplerState result = Allocate(LGFXSamplerStateImpl, 1);
    result->device = device;
    result->handle = wgpuDeviceCreateSampler((WGPUDevice)device->logicalDevice, &samplerDescriptor);
    return result;
}
void wLGFXDestroySamplerState(LGFXSamplerState state)
{
    wgpuSamplerRelease(state->handle);
    free(state);
}

LGFXRenderTarget wLGFXCreateRenderTarget(LGFXDevice device, LGFXRenderTargetCreateInfo *info)
{

}
void wLGFXDestroyRenderTarget(LGFXRenderTarget target)
{

}

LGFXBuffer wLGFXCreateBuffer(LGFXDevice device, LGFXBufferCreateInfo *info)
{
    WGPUBufferDescriptor bufferDescriptor = WGPU_BUFFER_DESCRIPTOR_INIT;
    bufferDescriptor.usage = LGFXBufferUsage2WebGPU(info->bufferUsage);
    bufferDescriptor.mappedAtCreation = WGPU_FALSE;
    bufferDescriptor.size = info->size;

    if (info->memoryUsage == LGFXMemoryUsage_CPU_TO_GPU)
    {
        bufferDescriptor.usage |= WGPUBufferUsage_MapWrite;
    }
    else if (info->memoryUsage == LGFXMemoryUsage_GPU_TO_CPU)
    {
        bufferDescriptor.usage |= WGPUBufferUsage_MapRead;
    }

    if (info->memoryIdentifierName != NULL)
    {
        bufferDescriptor.label.data = info->memoryIdentifierName;
        bufferDescriptor.label.length = WGPU_STRLEN;
    }

    LGFXBuffer result = Allocate(LGFXBufferImpl, 1);
    result->handle =  wgpuDeviceCreateBuffer((WGPUDevice)device->logicalDevice, &bufferDescriptor);
    result->bufferMemory = NULL;
    result->device = device;
    result->size = info->size;
    result->usage = info->bufferUsage;

    return result;
}
void wLGFXCopyBufferToBuffer(LGFXDevice device, LGFXCommandBuffer commandBuffer, LGFXBuffer from, LGFXBuffer to, size_t fromBufferOffset, size_t setIntoBufferOffset)
{
    WGPUDevice wDevice = (WGPUDevice)device->logicalDevice;
    WGPUCommandEncoder encoder;
    
    if (commandBuffer != NULL)
    {
        encoder = (WGPUCommandEncoder)commandBuffer->cmdBuffer;
    }
    else
    {
        encoder = wgpuDeviceCreateCommandEncoder(wDevice, NULL);
    }

    wgpuCommandEncoderCopyBufferToBuffer(encoder, (WGPUBuffer)from->handle, fromBufferOffset, (WGPUBuffer)to->handle, setIntoBufferOffset, from->size);

    if (commandBuffer == NULL)
    {
        WGPUCommandBuffer finalizedCommands = wgpuCommandEncoderFinish(encoder, NULL);
        wgpuQueueSubmit((WGPUQueue)device->graphicsQueue->queue, 1, &finalizedCommands);
        wgpuCommandEncoderRelease(encoder);
        wgpuCommandBufferRelease(finalizedCommands);
    }
}
void wLGFXSetBufferDataOptimizedData(LGFXBuffer buffer, LGFXCommandBuffer commandBufferToUse, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength)
{
    LGFXDevice device = buffer->device;
    wgpuQueueWriteBuffer((WGPUQueue)device->graphicsQueue->queue, (WGPUBuffer)buffer->handle, setIntoBufferOffset, data, dataLength);
}
void wLGFXSetBufferDataFast(LGFXBuffer buffer, uint8_t *data, size_t setIntoBufferOffset, size_t dataLength)
{
    LGFXDevice device = buffer->device;
    wgpuQueueWriteBuffer((WGPUQueue)device->graphicsQueue->queue, (WGPUBuffer)buffer->handle, setIntoBufferOffset, data, dataLength);
}
void wLGFXFillBuffer(LGFXCommandBuffer cmdBuffer, LGFXBuffer buffer, uint32_t value)
{
    LGFXDevice device = buffer->device;
    usize bufferSizeRound = (u32)(floorf(sizeof(buffer->size) / (float)sizeof(u32)) * sizeof(u32));
    u32 *fill = (u32 *)malloc(bufferSizeRound);
    memset(fill, value, bufferSizeRound);
    wgpuQueueWriteBuffer((WGPUQueue)device->graphicsQueue->queue, (WGPUBuffer)buffer->handle, 0, fill, bufferSizeRound);
    free(fill);
}
void wLGFXDestroyBuffer(LGFXBuffer buffer)
{
    wgpuBufferRelease(buffer);
}

void wLGFXMapBufferCallback(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2)
{
    if (status != WGPUMapAsyncStatus_Success)
    {
        *((u32 *)userdata1) = 0;
    }
}
bool wLGFXMapBuffer(LGFXBuffer buffer)
{
    u32 result = 1;
    WGPUBufferMapCallbackInfo callbackInfo = (WGPUBufferMapCallbackInfo){0};
    callbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.callback = &wLGFXMapBufferCallback;
    callbackInfo.userdata1 = &result;

    WGPUFuture future = wgpuBufferMapAsync((WGPUBuffer)buffer->handle, WGPUMapMode_Read, 0, buffer->size, callbackInfo);
    wLGFXAwaitFuture(future, buffer->device->instance);

    return result == 1;
}

void *wLGFXReadBufferFromGPU(LGFXBuffer buffer, void *(*allocateFunction)(size_t))
{
    WGPUDevice wDevice = (WGPUDevice)buffer->device->logicalDevice;

    WGPUBufferDescriptor descriptor = (WGPUBufferDescriptor){0};
    descriptor.mappedAtCreation = WGPU_TRUE;
    descriptor.size = buffer->size;
    descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
    WGPUBuffer tempBufferHandle = wgpuDeviceCreateBuffer(wDevice, &descriptor);

    LGFXBufferImpl tempBuffer = (LGFXBufferImpl){0};
    tempBuffer.device = buffer->device;
    tempBuffer.handle = tempBufferHandle;
    tempBuffer.size = buffer->size;

    bool mapped = wLGFXMapBuffer(&tempBuffer);
    if (mapped)
    {
        wLGFXCopyBufferToBuffer(buffer->device, NULL, buffer, &tempBuffer, 0, 0);

        const void *ptr = wgpuBufferGetConstMappedRange(tempBufferHandle, 0, WGPU_WHOLE_MAP_SIZE);
        void *result = allocateFunction(buffer->size);
        memcpy(result, ptr, buffer->size);

        wgpuBufferUnmap(tempBufferHandle);
        wgpuBufferRelease(tempBufferHandle);

        return result;
    }
    else return NULL;
}
void *wLGFXGetBufferData(LGFXBuffer buffer)
{
    bool mapped = wgpuBufferGetMapState((WGPUBuffer)buffer->handle) == WGPUBufferMapState_Mapped;
    if (!mapped)
    {
        mapped = wLGFXMapBuffer(buffer);
        if (!mapped)
        {
            return NULL;
        }
    }
    return wgpuBufferGetMappedRange((WGPUBuffer)buffer->handle, 0, WGPU_WHOLE_MAP_SIZE);
}
bool wLGFXBufferResize(LGFXBuffer buffer, size_t newSize)
{
	if (newSize == 0 || (buffer->usage & LGFXBufferUsage_TransferSource) == 0 || (buffer->usage & LGFXBufferUsage_TransferDest) == 0)
	{
		return false;
	}

    WGPUBuffer originalHandle = (WGPUBuffer)buffer->handle;
    void *data = wLGFXReadBufferFromGPU(buffer, &malloc);

    if (data == NULL)
    {
        return false;
    }

    WGPUBufferDescriptor bufferDescriptor = WGPU_BUFFER_DESCRIPTOR_INIT;
    bufferDescriptor.usage = LGFXBufferUsage2WebGPU(buffer->usage);
    bufferDescriptor.mappedAtCreation = WGPU_FALSE;
    bufferDescriptor.size = buffer->size;

    // if (buffer->bufferMemory != NULL)
    // {
    //     bufferDescriptor.label.data = info->memoryIdentifierName;
    //     bufferDescriptor.label.length = WGPU_STRLEN;
    // }

    free(data);

    buffer->size = newSize;
    wgpuBufferRelease(originalHandle);
    return true;
}