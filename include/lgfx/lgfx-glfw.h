#ifndef LGFX_GLFW_H
#define LGFX_GLFW_H

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

inline HWND LGFXGetNativeWindowHandle(GLFWwindow *window)
{
    return glfwGetWin32Window(window);
}

#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef MACOS
#error TODO
#endif

#ifdef LINUX
#error TODO
#endif

#endif