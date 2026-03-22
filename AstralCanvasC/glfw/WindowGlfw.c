#include "lgfx/lgfx-glfw.h"
#define WINDOW_TYPE GLFWwindow
#include "Window_c.h"

int32_t Windowing_InitializeBackend()
{
    #ifdef X11
    glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
    #endif
    return glfwInit();
}
const char **Windowing_GetRequiredInstanceExtensions(uint32_t *outExtensionsCount)
{
    return glfwGetRequiredInstanceExtensions(outExtensionsCount);
}
double Windowing_GetTime()
{
    return glfwGetTime();
}
void Windowing_CollectEvents()
{
    glfwPollEvents();
}
void Windowing_AwaitEvents()
{
    glfwWaitEvents();
}

static inline GLFWmonitor* GetCurrentMonitor(GLFWwindow *window)
{
    int32_t nmonitors, i;
    int32_t wx, wy, ww, wh;
    int32_t mx, my, mw, mh;
    int32_t overlap, bestoverlap;
	GLFWmonitor *bestmonitor = NULL;
	GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) 
	{
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            fmaxl(0, fminl(wx + ww, mx + mw) - fmaxl(wx, mx)) *
            fmaxl(0, fminl(wy + wh, my + mh) - fmaxl(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}

typedef struct Application Application;
static void Window_Deinit(Window *self)
{
    if (!self->isDisposed)
    {
        LGFXAwaitSwapchainIdle(self->swapchain);
        if (self->handle != NULL)
        {
            glfwDestroyWindow((GLFWwindow*)self->handle);
        }
        LGFXDestroySwapchain(self->swapchain, true);

        self->handle = NULL;
        self->isDisposed = true;
    }
}

void WindowHidden(GLFWwindow* window, int32_t iconified)
{
    if (iconified)
    {
        Window *canvas = (Window*)glfwGetWindowUserPointer(window);
        InputState_ResetAllInputStates(&canvas->windowInputState);
        //canvas->isMaximized = false;
    }
}
/// Called when the window is maximized or restored to original size
void WindowMaximized(GLFWwindow* window, int32_t maximized)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    glfwGetWindowSize(window, &canvas->resolution.X, &canvas->resolution.Y);
    canvas->isMaximized = maximized;
}
void WindowOnTextInput(GLFWwindow* window, uint32_t characterUnicode)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    List_Add(&canvas->windowInputState.textInputCharacters, &characterUnicode);

    if (canvas->onTextInputFunc != NULL)
    {
        canvas->onTextInputFunc(canvas, characterUnicode);
    }
}
void WindowSizeChanged(GLFWwindow *window, int32_t width, int32_t height)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    canvas->resolution.X = width;
    canvas->resolution.Y = height;
    canvas->justResized = true;
}
void WindowFramebufferSizeChanged(GLFWwindow* window, int32_t width, int32_t height)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    canvas->frameBufferSize.X = width;
    canvas->frameBufferSize.Y = height;
    canvas->justResized = true;
}
void WindowOnDrop(GLFWwindow* window, int count, const char** paths)
{
    Window* astralWindow = (Window*)glfwGetWindowUserPointer(window);
    if (astralWindow->onDropFunc)
    {
        astralWindow->onDropFunc(astralWindow, count, paths);
    }
}
void WindowOnClose(GLFWwindow* window)
{
    Window* astralWindow = (Window*)glfwGetWindowUserPointer(window);
    if (astralWindow->onCloseFunc)
    {
        astralWindow->onCloseFunc(astralWindow);
    }
    astralWindow->handle = NULL;
    Window_Deinit(astralWindow);
}
void WindowOnKeyInteracted(GLFWwindow* window, int32_t glfwKey, int32_t scancode, int32_t action, int32_t mods)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    if (glfwKey == GLFW_KEY_UNKNOWN)
    {
        return;
    }
    Keys key = (Keys)glfwKey;
    if (action == GLFW_PRESS)
    {
        KeyStateStatus status;
        status.perFrameState = KeyState_Pressed;
        status.status = true;
        
        canvas->windowInputState.keyStatuses[glfwKey] = status;
    }
    else if (action == GLFW_RELEASE)
    {
        KeyStateStatus status;
        status.perFrameState = KeyState_Released;
        status.status = false;

        canvas->windowInputState.keyStatuses[glfwKey] = status;
    }
    if (key == GLFW_KEY_BACKSPACE)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            const uint32_t backspaceChar = (uint32_t)'\b';
            List_Add(&canvas->windowInputState.textInputCharacters, &backspaceChar);
        }
    }
    if (canvas->onKeyInteractFunc != NULL)
    {
        canvas->onKeyInteractFunc(canvas, key, action);
    }
}
void WindowOnMouseInteracted(GLFWwindow *window, int32_t button, int32_t action, int32_t mods)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);

    MouseButtons btn = MouseButton_Other;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        btn = MouseButton_Left;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        btn = MouseButton_Right;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        btn = MouseButton_Middle;
    }

    if (action == GLFW_PRESS)
    {
        if (!canvas->windowInputState.mouseStatuses[btn].status)
        {
            KeyStateStatus status;
            status.perFrameState = KeyState_Pressed;
            status.status = true;
            canvas->windowInputState.mouseStatuses[btn] = status;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (!canvas->windowInputState.mouseStatuses[btn].status)
        {
            KeyStateStatus status;
            status.perFrameState = KeyState_Released;
            status.status = false;
            canvas->windowInputState.mouseStatuses[btn] = status;
        }
    }
}
void WindowOnMouseScrolled(GLFWwindow *window, double xPos, double yPos)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    canvas->windowInputState.scroll = CreateVec2((float)xPos, (float)yPos);
}
void WindowOnCursorMoved(GLFWwindow *window, double xPos, double yPos)
{
    Window *canvas = (Window*)glfwGetWindowUserPointer(window);
    canvas->windowInputState.mousePosition = CreateVec2((float)xPos, (float)yPos);
}

Window *Window_Create(Application *application, const WindowCreationInfo *createInfo)
{
    Window empty = {};
    Window *result = (Window *)malloc(sizeof(Window));
    *result = empty;

    int32_t width = createInfo->width;
    int32_t height = createInfo->height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, (int32_t)createInfo->resizeable);
    glfwWindowHint(GLFW_MAXIMIZED, (int32_t)createInfo->maximized);

    GLFWmonitor *toFullscreenOn = NULL;
    if (createInfo->fullscreen)
    {
        toFullscreenOn = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidMode = glfwGetVideoMode(toFullscreenOn);
        width = vidMode->width;
        height = vidMode->height;
    }

    result->handle = glfwCreateWindow(width, height, createInfo->name, toFullscreenOn, NULL);
    if (result->handle != NULL)
    {
        glfwSetWindowUserPointer(result->handle, result);
        if (createInfo->iconData != NULL)
        {
            GLFWimage image;
            image.width = createInfo->iconWidth;
            image.height = createInfo->iconHeight;
            image.pixels = (uint8_t *)createInfo->iconData;
            glfwSetWindowIcon(result->handle, 1, &image);
        }
        
        result->windowInputState = InputState_Create();
        result->resolution = CreatePoint2(width, height);
        result->isFullscreen = createInfo->fullscreen;
        result->isMaximized = createInfo->maximized;
        if (createInfo->name != NULL)
            result->windowTitle = StringFrom(GetCAllocator(), createInfo->name);

        glfwSetWindowIconifyCallback(result->handle, &WindowHidden);
        glfwSetWindowMaximizeCallback(result->handle, &WindowMaximized);
        glfwSetWindowSizeCallback(result->handle, &WindowSizeChanged);
        glfwSetFramebufferSizeCallback(result->handle, &WindowFramebufferSizeChanged);
        glfwSetCharCallback(result->handle, &WindowOnTextInput);
        glfwSetKeyCallback(result->handle, &WindowOnKeyInteracted);
        glfwSetMouseButtonCallback(result->handle, &WindowOnMouseInteracted);
        glfwSetScrollCallback(result->handle, &WindowOnMouseScrolled);
        glfwSetCursorPosCallback(result->handle, &WindowOnCursorMoved);
        glfwGetWindowPos(result->handle, &result->position.Y, &result->position.Y);
        glfwSetDropCallback(result->handle, &WindowOnDrop);
        glfwSetWindowShouldClose(result->handle, 0);
        glfwSetWindowCloseCallback(result->handle, &WindowOnClose);

        //init swapchain here
        //create swapchain
        LGFXSwapchainCreateInfo swapchainCreateInfo = {0};
        swapchainCreateInfo.oldSwapchain = NULL;
        swapchainCreateInfo.presentationMode = createInfo->presentMode;
        result->isVSync = swapchainCreateInfo.presentationMode == LGFXSwapchainPresentationMode_Fifo;
        
        glfwGetFramebufferSize(result->handle, &result->frameBufferSize.X, &result->frameBufferSize.Y);
        swapchainCreateInfo.width = (uint32_t)result->frameBufferSize.X;
        swapchainCreateInfo.height = (uint32_t)result->frameBufferSize.Y;
        swapchainCreateInfo.createSurfaceFunc = (LGFXCreateWindowSurfaceFunc)&glfwCreateWindowSurface;
        swapchainCreateInfo.windowHandle = result->handle;//nativeWindowHandle = LGFXGetNativeWindowHandle(handle);
        //swapchainCreateInfo.displayHandle = LGFXGetNativeWindowDisplay(handle);

        result->swapchain = LGFXCreateSwapchain(application->device, &swapchainCreateInfo);
        result->mainCommandBuffer = LGFXCreateCommandBuffer(application->device, false);

        List_Add(&application->windows, &result);
        return result;
    }

    return NULL;
}

void Window_SetTitle(Window *self, const char *title)
{
    if (self->windowTitle.buffer != NULL)
    {
        String_Deinit(&self->windowTitle);
    }
    self->windowTitle = StringFrom(GetCAllocator(), title);

    glfwSetWindowTitle(self->handle, title);
}
void Window_SetMaximized(Window *self, bool value)
{
    if (value)
    {
        glfwMaximizeWindow((GLFWwindow *)self->handle);
    }
    else
    {
        glfwRestoreWindow((GLFWwindow *)self->handle);
    }
}
void Window_SetFullscreen(Window *self, bool value)
{
    if (value)
    {
        GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow *)self->handle);
        if (monitor == NULL)
        {
            monitor = glfwGetPrimaryMonitor();
        }
        int32_t xpos;
        int32_t ypos;
        int32_t w;
        int32_t h;
        glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &w, &h);

        const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor((GLFWwindow*)self->handle, monitor, xpos, ypos, w, h, videoMode->refreshRate);
        // self->resolution.X = videoMode->width;
        // self->resolution.Y = videoMode->height;
    }
    else
    {
        glfwSetWindowMonitor((GLFWwindow*)self->handle, NULL, self->position.X, self->position.Y, self->resolution.X, self->resolution.Y, GLFW_DONT_CARE);
    }
    self->isFullscreen = value;
}
void Window_SetResolution(Window *self, uint32_t width, uint32_t height)
{
    if (self->isFullscreen)
    {
        return;
    }
    int32_t w;
    int32_t h;
    GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow *)self->handle);
    glfwGetMonitorWorkarea(monitor, NULL, NULL, &w, &h);

    glfwSetWindowSize((GLFWwindow *)self->handle, (uint32_t)width, (uint32_t)height);
    if (self->isMaximized)
    {
        glfwRestoreWindow((GLFWwindow *)self->handle);
        self->isMaximized = false;
    }
}
void Window_SetPosition(Window *self, float X, float Y)
{
	glfwSetWindowPos((GLFWwindow *)self->handle, X, Y);
}
void Window_SetMousePosition(Window *self, float relativeX, float relativeY)
{
    glfwSetCursorPos((GLFWwindow*)self->handle, (double)relativeX, (double)relativeY);
}
void Window_SetMouseState(Window *self, WindowMouseState value)
{
    if (value == WindowMouseState_Default)
    {
        glfwSetInputMode((GLFWwindow *)self->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (value == WindowMouseState_Disabled)
    {
        glfwSetInputMode((GLFWwindow *)self->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    { 
        glfwSetInputMode((GLFWwindow *)self->handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}
const char *Window_GetTitle(const Window *self)
{
    return self->windowTitle.buffer;
}
bool Window_IsMaximized(const Window *self)
{
    return self->isMaximized;
}
bool Window_IsFullscreen(const Window *self)
{
    return self->isFullscreen;
}
Point2 Window_GetResolution(const Window *self)
{
    Point2 result;
    glfwGetWindowSize(self->handle, &result.X, &result.Y);
    return result;
}
Vec2 Window_GetPosition(const Window *self)
{
    int32_t posX;
    int32_t posY;
    glfwGetWindowPos(self->handle, &posX, &posY);

    return CreateVec2(posX, posY);
}
WindowMouseState Window_GetMouseState(const Window *self)
{
    int32_t cursorMode = glfwGetInputMode(self->handle, GLFW_CURSOR);
    if (cursorMode == GLFW_CURSOR_NORMAL)
    {
        return WindowMouseState_Default;
    }
    else if (cursorMode == GLFW_CURSOR_HIDDEN)
    {
        return WindowMouseState_Hidden;
    }
    else
    {
        return WindowMouseState_Disabled;
    }
}
LGFXSwapchain Window_GetSwapchain(const Window *self)
{
    return self->swapchain;
}
LGFXCommandBuffer Window_GetMainCommandBuffer(const Window *self)
{
    return self->mainCommandBuffer;
}
Vec2 Window_GetFramebufferSize(const Window *self)
{
    int sizeX;
    int sizeY;
    glfwGetFramebufferSize(self->handle, &sizeX, &sizeY);
    return CreateVec2(sizeX, sizeY);
}
Vec2 Window_GetOSScale(Window *self)
{
    Vec2 result;
    glfwGetWindowContentScale((GLFWwindow *)self->handle, &result.X, &result.Y);
    return result;
}
void Window_SetIcon(Window *self, void *iconData, uint32_t iconWidth, uint32_t iconHeight, int32_t originX, int32_t originY)
{
    GLFWimage iconImg;
    iconImg.width = iconWidth;
    iconImg.height = iconHeight;
    iconImg.pixels = (uint8_t *)iconData;
    glfwSetWindowIcon(self->handle, 1, &iconImg);
}
void Window_Close(Window *self)
{
    glfwSetWindowShouldClose((GLFWwindow *)self->handle, GLFW_TRUE);
}
void Window_InterceptClose(Window *self)
{
    glfwSetWindowShouldClose((GLFWwindow*)self->handle, GLFW_FALSE);
}
void *Window_GetOSHandle(const Window *self)
{
#if WINDOWS
		return glfwGetWin32Window((GLFWwindow*)self->handle);
#endif
#if LINUX
#ifdef X11
		if (glfwGetPlatform() == GLFW_PLATFORM_X11)
		{
			return (void *)glfwGetX11Window((GLFWwindow *)self->handle);
		}
#endif
#ifdef WAYLAND
		if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
		{
			return glfwGetWaylandWindow((GLFWwindow *)self->handle);
		}
#endif
#endif
#if MACOS
		return glfwGetCocoaWindow((GLFWwindow *)self->handle);
#endif
return NULL;
}
int32_t Window_GetCurrentMonitorFramerate(const Window *self)
{
    GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow*)self->handle);
    const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

    return vidmode->refreshRate;
}
Vec2 Window_GetCurrentMonitorResolution(const Window *self)
{
    GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow*)self->handle);
    int32_t w;
    int32_t h;
    glfwGetMonitorWorkarea(monitor, NULL, NULL, &w, &h);

    return CreateVec2((float)w, (float)h);
}