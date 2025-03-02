#ifndef LGFX_GLFW_H
#define LGFX_GLFW_H

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static inline HWND LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    return glfwGetWin32Window(window);
}
static inline void *LGFXGetNativeWindowDisplay()
{
    return NULL;
}

#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef MACOS
#error TODO
#endif

#ifdef X11
inline void *LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    Window handle = glfwGetX11Window(window);
    return (void *)handle;
}
static inline void *LGFXGetNativeWindowDisplay()
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
static inline void *LGFXGetNativeWindowDisplay()
{
    wl_display *display = glfwGetWaylandDisplay();
    return (void *)display;
}
#endif

#endif