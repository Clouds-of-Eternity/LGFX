#pragma once
#include "Maths/Points.h"
#include "lgfx/lgfx.h"
#include "AstralCanvas/InputState.h"
#include "Strings.h"

typedef enum WindowMouseState
{
    WindowMouseState_Default,
    WindowMouseState_Hidden,
    WindowMouseState_Disabled
} WindowMouseState;

typedef struct Window Window;

def_delegate(WindowOnTextInputFunction, void, Window *window, uint32_t characterUnicode);
def_delegate(WindowOnKeyInteractedFunction, void, Window *window, Keys key, int32_t action);
def_delegate(WindowOnDropFunction, void, Window *window, int count, const char** paths);
def_delegate(WindowCloseFunction, void, Window *window);

typedef struct WindowCreationInfo
{
    const char *name;
    int32_t width;
    int32_t height;
    bool resizeable;
    bool maximized;
    bool fullscreen;
    LGFXSwapchainPresentationMode presentMode;

    void *iconData;
    uint32_t iconWidth;
    uint32_t iconHeight;
} WindowCreationInfo;

DynamicFunction Window *Window_Create(const WindowCreationInfo *createInfo);

DynamicFunction bool Window_Update(Window *self);

DynamicFunction void Window_SetTitle(Window *self, const char *title);
DynamicFunction void Window_SetMaximized(Window *self, bool value);
DynamicFunction void Window_SetFullscreen(Window *self, bool value);
DynamicFunction void Window_SetResolution(Window *self, uint32_t width, uint32_t height);
DynamicFunction void Window_SetPosition(Window *self, float X, float Y);
DynamicFunction void Window_SetMousePosition(Window *self, float relativeX, float relativeY);
DynamicFunction void Window_SetMouseState(Window *self, WindowMouseState value);

DynamicFunction const char *Window_GetTitle(const Window *self);
DynamicFunction bool Window_IsMaximized(const Window *self);
DynamicFunction bool Window_IsFullscreen(const Window *self);
DynamicFunction Point2 Window_GetResolution(const Window *self);
DynamicFunction Vec2 Window_GetPosition(const Window *self);
DynamicFunction WindowMouseState Window_GetMouseState(const Window *self);
DynamicFunction LGFXSwapchain Window_GetSwapchain(const Window *self);
DynamicFunction LGFXCommandBuffer Window_GetMainCommandBuffer(const Window *self);

DynamicFunction Vec2 Window_GetFramebufferSize(const Window *self);
DynamicFunction Vec2 Window_GetOSScale(Window *self);

DynamicFunction void Window_SetIcon(Window *self, void *iconData, uint32_t iconWidth, uint32_t iconHeight, int32_t originX, int32_t originY);

DynamicFunction void Window_Close(Window *self);
DynamicFunction void Window_InterceptClose(Window *self);

DynamicFunction void *Window_GetOSHandle(const Window *self);
DynamicFunction int32_t Window_GetCurrentMonitorFramerate(const Window *self);
DynamicFunction Vec2 Window_GetCurrentMonitorResolution(const Window *self);