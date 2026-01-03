#ifndef LGFX_GLFW_H
#define LGFX_GLFW_H


#ifdef WINDOWS
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#endif

#include "vulkan/vulkan.h"

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static inline HWND LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    return glfwGetWin32Window(window);
}
static inline void *LGFXGetNativeWindowDisplay(GLFWwindow *window)
{
    return NULL;
}

#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef MACOS
inline void *LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    return glfwGetCocoaWindow(window);
}
static inline void *LGFXGetNativeWindowDisplay(GLFWwindow *window)
{
    return glfwGetCocoaView(window);
}
#endif

#ifdef X11
inline void *LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    Window handle = glfwGetX11Window(window);
    return (void *)handle;
}
static inline void *LGFXGetNativeWindowDisplay(GLFWwindow *window)
{
    Display *display = glfwGetX11Display();
    return (void *)display;
}
#elif defined(WAYLAND)
inline void *LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    wl_surface *handle = glfwGetWaylandWindow(window);
    return (void *)handle;
}
static inline void *LGFXGetNativeWindowDisplay(GLFWwindow *window)
{
    wl_display *display = glfwGetWaylandDisplay();
    return (void *)display;
}
#endif

#endif