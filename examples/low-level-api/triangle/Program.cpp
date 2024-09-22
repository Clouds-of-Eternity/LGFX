#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Linxc.h"
#include "stdio.h"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"

LGFXInstance instance;
LGFXDevice device;
LGFXSwapchain swapchain;

i32 main()
{
    printf("Hello World!\n");
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

    //main loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    //shutdown
    LGFXDestroySwapchain(swapchain);

    LGFXDestroyDevice(device);

    LGFXDestroyInstance(instance);
}