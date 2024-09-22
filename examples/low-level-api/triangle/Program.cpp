#include "GLFW/glfw3.h"
#include "Linxc.h"
#include "stdio.h"
#include "lgfx/lgfx.h"

LGFXInstance instance;
LGFXDevice device;

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

    //main loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    //shutdown
    LGFXDestroyDevice(device);

    LGFXDestroyInstance(instance);
}