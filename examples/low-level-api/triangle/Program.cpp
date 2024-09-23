#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Linxc.h"
#include "stdio.h"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"

LGFXInstance instance;
LGFXDevice device;
LGFXSwapchain swapchain;
LGFXCommandBuffer mainCommands;
LGFXRenderProgram rp;

i32 main()
{
    glfwInit();

    //create window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "LGFX Triangle!", NULL, NULL);

    //create instance
    u32 extensionsCount;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    LGFXInstanceCreateInfo instanceCreateInfo = {0};
    instanceCreateInfo.appName = "LGFX Triangle";
    instanceCreateInfo.appVersion = 0;
    instanceCreateInfo.engineName = "None";
    instanceCreateInfo.engineVersion = 0;
    instanceCreateInfo.runtimeErrorChecking = true;
    instanceCreateInfo.backend = LGFXBackendType_Vulkan;
    instanceCreateInfo.enabledExtensionsCount = extensionsCount;
    instanceCreateInfo.enabledExtensions = extensions;

    instance = LGFXCreateInstance(&instanceCreateInfo);

    //create device
    LGFXDeviceCreateInfo deviceCreateInfo = {0};
    device = LGFXCreateDevice(instance, &deviceCreateInfo);

    //create swapchain
    LGFXSwapchainCreateInfo swapchainCreateInfo = {0};
    swapchainCreateInfo.oldSwapchain = NULL;
    swapchainCreateInfo.presentationMode = LGFXSwapchainPresentationMode_Mailbox;
    int w;
    int h;
    glfwGetFramebufferSize(window, &w, &h);
    swapchainCreateInfo.width = (u32)w;
    swapchainCreateInfo.height = (u32)h;
    swapchainCreateInfo.nativeWindowHandle = LGFXGetNativeWindowHandle(window);

    swapchain = LGFXCreateSwapchain(device, &swapchainCreateInfo);

    //create main command buffer to reuse
    mainCommands = LGFXCreateCommandBuffer(device, false);

    //render program
    LGFXRenderAttachmentInfo attachments;
    attachments.clear = true;
    attachments.format = LGFXTextureFormat_BGRA8Unorm;
    attachments.readByRenderTarget = false;

    i32 firstAttachment = 0;

    LGFXRenderPassInfo passes;
    passes.colorAttachmentIDs = &firstAttachment;
    passes.colorAttachmentsCount = 1;
    passes.depthAttachmentID = -1;
    passes.readAttachmentIDs = NULL;
    passes.readAttachmentsCount = 0;

    LGFXRenderProgramCreateInfo rpCreateInfo;
    rpCreateInfo.attachmentsCount = 1;
    rpCreateInfo.attachments = &attachments;
    rpCreateInfo.renderPassCount = 1;
    rpCreateInfo.renderPasses = &passes;
    rp = LGFXCreateRenderProgram(device, &rpCreateInfo);

    //main loop
    while (!glfwWindowShouldClose(window)) 
    {
        int framebufferWidth;
        int framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        // if (LGFXNewFrame(device, &swapchain, (u32)framebufferWidth, (u32)framebufferHeight))
        // {

        //     LGFXSubmitFrame(device, &swapchain, (u32)framebufferWidth, (u32)framebufferHeight);
        // }

        //LGFXSubmitFrame(device, &swapchain, (u32)framebufferWidth, (u32)framebufferHeight);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    //shutdown
    LGFXDestroyRenderProgram(rp);

    LGFXDestroySwapchain(swapchain);

    LGFXDestroyDevice(device);

    LGFXDestroyInstance(instance);
}