#include <stdio.h>
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "lgfx/lgfx.h"
#include "lgfx/lgfx-glfw.h"
#include "lgfx-astral/Application.hpp"

using namespace Maths;

GLFWmonitor* GetCurrentMonitor(GLFWwindow *window)
{
    i32 nmonitors, i;
    i32 wx, wy, ww, wh;
    i32 mx, my, mw, mh;
    i32 overlap, bestoverlap;
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

namespace AstralCanvas
{
	Window::Window()
	{
		this->handle = NULL;
		this->position = Point2(0);
		this->resolution = Point2(0);
		this->windowInputState = {};
		this->onTextInputFunc = NULL;
		this->onKeyInteractFunc = NULL;
        this->onDropFunc = NULL;
        this->onCloseFunc = NULL;
		this->swapchain = NULL;
        this->mainCommandBuffer = NULL;
        this->isDisposed = false;
		this->justResized = NULL;
	}

	void Window::deinit()
	{
		if (!isDisposed)
		{
            LGFXAwaitSwapchainIdle(swapchain);
			glfwDestroyWindow((GLFWwindow*)this->handle);
            LGFXDestroySwapchain(swapchain, true);

			this->handle = NULL;
			isDisposed = true;
		}
	}
	void Window::CloseWindow()
	{
		glfwSetWindowShouldClose((GLFWwindow *)this->handle, GLFW_TRUE);
		deinit();
	}

    void Window::InterceptClose()
    {
        glfwSetWindowShouldClose((GLFWwindow*)this->handle, GLFW_FALSE);
    }

	void Window::SetResolution(u32 width, u32 height)
	{
		glfwSetWindowSize((GLFWwindow *)handle, (u32)width, (u32)height);
	}
	void Window::SetPosition(float posX, float posY)
	{
		glfwSetWindowPos((GLFWwindow *)handle, posX, posY);
	}
	void Window::SetMousePosition(float posX, float posY)
	{
		glfwSetCursorPos((GLFWwindow*)handle, (double)posX, (double)posY);
	}
	void Window::SetMouseState(WindowMouseState state)
	{
		if (state == AstralCanvas::WindowMouseState_Default)
		{
			glfwSetInputMode((GLFWwindow *)handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (state == AstralCanvas::WindowMouseState_Disabled)
		{
			glfwSetInputMode((GLFWwindow *)handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{ 
			glfwSetInputMode((GLFWwindow *)handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}
	Maths::Vec2 Window::GetOSContentScale()
	{
		Maths::Vec2 result = {};
		glfwGetWindowContentScale((GLFWwindow *)this->handle, &result.X, &result.Y);
		return result;
	}
	WindowMouseState Window::GetMouseState()
	{
		i32 mouseState = glfwGetInputMode((GLFWwindow *)handle, GLFW_CURSOR);

		switch (mouseState)
		{
			case GLFW_CURSOR_NORMAL:
				return WindowMouseState_Default;
			case GLFW_CURSOR_DISABLED:
				return WindowMouseState_Disabled;
			default:
				return WindowMouseState_Hidden;
		}
	}
	void Window::SetMouseIcon(void *iconData, u32 iconWidth, u32 iconHeight, i32 originX, i32 originY)
	{
		if (this->customCursorHandle == NULL)
		{
			glfwDestroyCursor((GLFWcursor*)this->customCursorHandle);
			this->customCursorHandle = NULL;
		}
		if (iconData != NULL)
		{
			GLFWimage image;
			image.pixels = (u8 *)iconData;
			image.width = iconWidth;
			image.height = iconHeight;
			this->customCursorHandle = glfwCreateCursor(&image, originX, originY);
			glfwSetCursor((GLFWwindow *)this->handle, (GLFWcursor *)this->customCursorHandle);
		}
		else
		{
			glfwSetCursor((GLFWwindow *)this->handle, NULL);
		}
	}
	/// Called when the window is hidden
	void WindowHidden(GLFWwindow* window, i32 iconified)
	{
		if (iconified)
		{
			Window *canvas = (Window*)glfwGetWindowUserPointer(window);
			canvas->windowInputState.ClearAllInputStates();
			canvas->isMaximized = false;
		}
	}
	/// Called when the window is maximized or restored to original size
	void WindowMaximized(GLFWwindow* window, i32 maximized)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		glfwGetWindowSize(window, &canvas->resolution.X, &canvas->resolution.Y);
		canvas->isMaximized = (bool)maximized;
	}
	void OnTextInput(GLFWwindow* window, u32 characterUnicode)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.textInputCharacter = characterUnicode;

		if (canvas->onTextInputFunc != NULL)
		{
			canvas->onTextInputFunc(canvas, characterUnicode);
		}
	}
	void OnKeyInteracted(GLFWwindow* window, i32 glfwKey, i32 scancode, i32 action, i32 mods)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		if (glfwKey == GLFW_KEY_UNKNOWN)
		{
			return;
		}
		AstralCanvas::Keys key = (AstralCanvas::Keys)glfwKey;
		if (action == GLFW_PRESS)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Pressed;
			status.status = true;
			canvas->windowInputState.keyStatuses.Insert(glfwKey, status);
		}
		else if (action == GLFW_RELEASE)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Released;
			status.status = false;
			canvas->windowInputState.keyStatuses.Insert(glfwKey, status);
		}
		if (key == GLFW_KEY_BACKSPACE)
		{
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				canvas->windowInputState.textInputCharacter = '\b';
			}
		}
		if (canvas->onKeyInteractFunc != NULL)
		{
			canvas->onKeyInteractFunc(canvas, key, action);
		}
	}
	void OnMouseInteracted(GLFWwindow *window, i32 button, i32 action, i32 mods)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);

		AstralCanvas::MouseButtons btn = MouseButton_Other;
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
			if (canvas->windowInputState.mouseStatuses.Get(btn) == NULL || !canvas->windowInputState.mouseStatuses.Get(btn)->status)
			{
				AstralCanvas::KeyStateStatus status;
				status.perFrameState = AstralCanvas::KeyState_Pressed;
				status.status = true;
				canvas->windowInputState.mouseStatuses.Insert(btn, status);
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (canvas->windowInputState.mouseStatuses.Get(btn) != NULL && canvas->windowInputState.mouseStatuses.Get(btn)->status)
			{
				AstralCanvas::KeyStateStatus status;
				status.perFrameState = AstralCanvas::KeyState_Released;
				status.status = false;
				canvas->windowInputState.mouseStatuses.Insert(btn, status);
			}
		}
	}
	void OnMouseScrolled(GLFWwindow *window, double xPos, double yPos)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.scroll = Maths::Vec2((float)xPos, (float)yPos);
	}
	void OnCursorMoved(GLFWwindow *window, double xPos, double yPos)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.mousePosition = Maths::Vec2((float)xPos, (float)yPos);
	}
    void WindowSizeChanged(GLFWwindow *window, i32 width, i32 height)
    {
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
        canvas->resolution.X = width;
        canvas->resolution.Y = height;
		canvas->justResized = true;
	}
	void WindowFramebufferSizeChanged(GLFWwindow* window, i32 width, i32 height)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
        canvas->resolution.X = width;
        canvas->resolution.Y = height;
		canvas->justResized = true;
	}

    void OnDrop(GLFWwindow* window, int count, const char** paths)
    {
		Window* astralWindow = (Window*)glfwGetWindowUserPointer(window);
        if (astralWindow->onDropFunc)
        {
            astralWindow->onDropFunc(astralWindow, count, paths);
        }
    }

    void OnWindowClose(GLFWwindow* window)
    {
        Window* astralWindow = (Window*)glfwGetWindowUserPointer(window);
        if (astralWindow->onCloseFunc)
        {
            astralWindow->onCloseFunc(astralWindow);
        }
    }

	bool WindowInit(IAllocator allocator, const char *name, Window * result, i32 width, i32 height, bool resizeable, bool maximized, bool fullscreen, void *iconData, u32 iconWidth, u32 iconHeight)
	{
		*result = {};
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, (i32)resizeable);
		glfwWindowHint(GLFW_MAXIMIZED, (i32)maximized);

		GLFWmonitor *toFullscreenOn = NULL;
		if (fullscreen)
		{
			toFullscreenOn = glfwGetPrimaryMonitor();
		}
		
		GLFWwindow* handle = glfwCreateWindow(width, height, name, toFullscreenOn, NULL);

		if (iconData != NULL)
		{
			GLFWimage image;
			image.width = iconWidth;
			image.height = iconHeight;
			image.pixels = (u8 *)iconData;
			glfwSetWindowIcon(handle, 1, &image);
		}

		if (handle)
		{
			*result = Window();
			result->windowInputState = AstralCanvas::InputState(allocator);
			result->handle = handle;
			result->resolution = Point2(width, height);
			result->isFullscreen = fullscreen;
			result->isMaximized = maximized;

			glfwSetWindowIconifyCallback(handle, &WindowHidden);
			glfwSetWindowMaximizeCallback(handle, &WindowMaximized);
            glfwSetWindowSizeCallback(handle, &WindowSizeChanged);
			glfwSetFramebufferSizeCallback(handle, &WindowFramebufferSizeChanged);
			glfwSetCharCallback(handle, &OnTextInput);
			glfwSetKeyCallback(handle, &OnKeyInteracted);
			glfwSetMouseButtonCallback(handle, &OnMouseInteracted);
			glfwSetScrollCallback(handle, &OnMouseScrolled);
			glfwSetCursorPosCallback(handle, &OnCursorMoved);
			glfwGetWindowPos(handle, &result->position.Y, &result->position.Y);
            glfwSetDropCallback(handle, &OnDrop);
            glfwSetWindowShouldClose(handle, 0);
            glfwSetWindowCloseCallback(handle, &OnWindowClose);

			//init swapchain here
			//create swapchain
			LGFXSwapchainCreateInfo swapchainCreateInfo = {0};
			swapchainCreateInfo.oldSwapchain = NULL;
			swapchainCreateInfo.presentationMode = LGFXSwapchainPresentationMode_Fifo;
			int w;
			int h;
			glfwGetFramebufferSize(handle, &w, &h);
			swapchainCreateInfo.width = (u32)w;
			swapchainCreateInfo.height = (u32)h;
			swapchainCreateInfo.nativeWindowHandle = LGFXGetNativeWindowHandle(handle);
			swapchainCreateInfo.displayHandle = LGFXGetNativeWindowDisplay();

			result->swapchain = LGFXCreateSwapchain(applicationInstance.device, &swapchainCreateInfo);
			result->mainCommandBuffer = LGFXCreateCommandBuffer(applicationInstance.device, false);

			return true;
		}

		return false;
	}

	void Window::SetWindowTitle(string title)
	{
		glfwSetWindowTitle((GLFWwindow*)handle, title.buffer);
		windowTitle = title;
	}
	i32 Window::GetCurrentMonitorFramerate()
	{
		GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow*)handle);
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

		return vidmode->refreshRate;
	}
	void Window::SetMaximized(bool value)
	{
		if (value)
		{
			glfwMaximizeWindow((GLFWwindow *)handle);
		}
		else
		{
			glfwRestoreWindow((GLFWwindow *)handle);
		}
	}
	void Window::SetFullscreen(bool value)
	{
		if (value)
		{
			GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow *)handle);
			if (monitor == NULL)
			{
				monitor = glfwGetPrimaryMonitor();
			}
			i32 xpos;
			i32 ypos;
			i32 w;
			i32 h;
			glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &w, &h);

			const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor((GLFWwindow*)handle, monitor, xpos, ypos, w, h, videoMode->refreshRate);
			resolution.X = videoMode->width;
			resolution.Y = videoMode->height;
		}
		else
		{
			glfwSetWindowMonitor((GLFWwindow*)handle, NULL, position.X, position.Y, resolution.X, resolution.Y, GLFW_DONT_CARE);
		}
		isFullscreen = value;
	}
}
