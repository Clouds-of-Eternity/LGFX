#include "Application_c.h"

Application instance;

Application *AstralCanvas_GetApplication()
{
    return &instance;
}
LGFXDevice AstralCanvas_GetDevice()
{
    return instance.device;
}
LGFXInstance AstralCanvas_GetGraphicsInstance()
{
    return instance.instance;
}

void AstralCanvas_Init(const char *appName, const char *engineName, uint32_t appVersion, uint32_t engineVersion, bool noWindow)
{
    instance.windows = List_Create(GetCAllocator(), sizeof(Window *));
    instance.currentWindow = NULL;
    instance.appName = StringFrom(GetCAllocator(), appName);
    instance.engineName = StringFrom(GetCAllocator(), engineName);
    instance.appVersion = appVersion;
    instance.engineVersion = engineVersion;
    
	instance.timeScale = 1.0f;
	instance.fixedTimeStep = 0.02f;
    instance.shouldShutdown = false;
    instance.shouldResetDeltaTimer = false;

    if (!noWindow)
    {
        #ifdef X11
        glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
        #endif
        glfwInit();
        u32 extensionsCount;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

        LGFXInstanceCreateInfo instanceCreateInfo = {0};
        instanceCreateInfo.appName = appName;
        instanceCreateInfo.appVersion = appVersion;
        instanceCreateInfo.engineName = engineName;
        instanceCreateInfo.engineVersion = engineVersion;
#if DEBUG
        instanceCreateInfo.runtimeErrorChecking = true;
#else
        instanceCreateInfo.runtimeErrorChecking = false;
#endif
        instanceCreateInfo.backend = LGFXBackendType_Vulkan;
        instanceCreateInfo.enabledExtensionsCount = extensionsCount;
        instanceCreateInfo.enabledExtensions = extensions;

        instance.instance = LGFXCreateInstance(&instanceCreateInfo);

        //create device
        LGFXDeviceCreateInfo deviceCreateInfo = {0};
        deviceCreateInfo.requiredFeatures.fillModeNonSolid = true;
        //deviceCreateInfo.requiredFeatures.wideLines = true;
        instance.device = LGFXCreateDevice(instance.instance, &deviceCreateInfo);
    }
}

void AstralCanvas_Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction fixedUpdateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationVoidFunction initFunc, ApplicationVoidFunction deinitFunc)
{
    instance.currentWindow = instance.windows.count > 0 ? LIST_GET(&instance.windows, Window *, 0) : NULL;
    if (initFunc != NULL)
    {
        initFunc();
    }
    instance.startTime = (float)glfwGetTime();
    instance.endTime = instance.startTime;
    bool noWindows = instance.windows.count == 0;
    if (noWindows)
    {
        instance.alwaysUpdate = true;
    }

    bool shouldStop = false;
    while (!shouldStop)
    {
        float deltaTime = instance.endTime - instance.startTime;

        glfwPollEvents();
        instance.updateTimer += deltaTime;
        instance.fixedUpdateTimer += deltaTime;

        //bool runUpdate = instance.framesPerSecond < 1.0f || instance.updateTimer >= 1.0f / framesPerSecond;
        bool minimized = instance.windows.count > 0;
        for (u32 i = 0; i < instance.windows.count; i++)
        {
            Window *window = LIST_GET(&instance.windows, Window *, i);
            Point2 res = Window_GetResolution(window);

            if (res.X > 0 && res.Y > 0)
            {
                minimized = false;
                break;
            }
        }
        if (minimized)
        {
            glfwWaitEvents();
            instance.endTime = (float)glfwGetTime();
        }

        //fixed update
        if (instance.fixedUpdateTimer > instance.fixedTimeStep * 4.0f) //cap to avoid death spiral
        {
            instance.fixedUpdateTimer = instance.fixedTimeStep * 4.0f;
        }
        while (instance.fixedUpdateTimer > instance.fixedTimeStep)
        {
            if (!minimized || instance.alwaysUpdate)
            {
                fixedUpdateFunc(instance.fixedTimeStep);
            }
            instance.fixedUpdateTimer -= instance.fixedTimeStep;
        }

        for (int32_t i = (int32_t)instance.windows.count - 1; i >= 0; i--)
        {
            Window *window = LIST_GET(&instance.windows, Window *, i);
            Point2 resolution = Window_GetResolution(window);
            Vec2 frameSize = Window_GetFramebufferSize(window);
            
            if (resolution.X == 0 || resolution.Y == 0 || frameSize.X == 0 || frameSize.Y == 0)
            {
                continue;
            }
            instance.currentWindow = window;
            if (!minimized || instance.alwaysUpdate)
            {
                updateFunc(instance.updateTimer * instance.timeScale);
            }

            bool hasClosed = Window_Update(window);
            if (hasClosed)
            {
                List_RemoveAtPullback(&instance.windows, (size_t)i);
                continue;
            }

            //begin draw
            LGFXSwapchain swapchain = Window_GetSwapchain(window);
            LGFXCommandBuffer mainCommandBuffer = Window_GetMainCommandBuffer(window);

            if (LGFXNewFrame(instance.device, swapchain, (u32)frameSize.X, (u32)frameSize.Y))
            {
                LGFXCommandBufferReset(mainCommandBuffer);
                LGFXCommandBufferBegin(mainCommandBuffer, true);

                drawFunc(deltaTime, &window);

                //end draw
                LGFXCommandBufferEndSwapchain(mainCommandBuffer, swapchain);
                LGFXSubmitFrame(instance.device, swapchain);

                if (postEndDrawFunc != NULL)
                {
                    postEndDrawFunc(deltaTime);
                }

                // for (u32 i = 0; i < allUsedShaders.count; i++)
                // {
                //     ShaderFunct
                //     allUsedShaders.ptr[i]->descriptorForThisDrawCall = 0;
                // }
                // List_Clear(&allUsedShaders);
            }
        }

        if (instance.windows.count == 0 && !noWindows)
        {
            break;
        }
        instance.updateTimer = 0.0f;
        

        instance.startTime = instance.endTime;
        instance.endTime = (float)glfwGetTime();
        if (instance.shouldResetDeltaTimer)
        {
            instance.startTime = instance.endTime;
            instance.shouldResetDeltaTimer = false;
        }
        if (instance.shouldShutdown)
        {
            shouldStop = true;
            for (usize i = 0; i < instance.windows.count; i++)
            {
                Window *window = LIST_GET(&instance.windows, Window *, i);
                Window_Close(window);
            }
        }
    }

    //await rendering process shutdown

    if (deinitFunc != NULL)
    {
        deinitFunc();
    }
    List_Deinit(&instance.windows);

    //deinitialize backend
    if (instance.device != NULL)
    {
        LGFXDestroyDevice(instance.device);
    }
    if (instance.instance != NULL)
    {
        LGFXDestroyInstance(instance.instance);
    }
}