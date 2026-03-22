#pragma once
#include "Application_c.h"
#include "lgfx/lgfx.h"
#include "Maths/Points.h"
#include "Strings.h"
#include "Maths/Vectors.h"

#ifndef WINDOW_TYPE
#define WINDOW_TYPE void
#endif

typedef struct Window
{
    WINDOW_TYPE *handle;
    void *customCursorHandle;
    LGFXSwapchain swapchain;
    LGFXCommandBuffer mainCommandBuffer;

    Point2 resolution;
    Point2 frameBufferSize;
    Point2 position;
    InputState windowInputState;
    string windowTitle;
    bool isFullscreen;
    bool isMaximized;
    bool isVSync;
    bool justResized;
    bool isDisposed;

    WindowOnTextInputFunction onTextInputFunc;
    WindowOnKeyInteractedFunction onKeyInteractFunc;
    WindowOnDropFunction onDropFunc;
    WindowCloseFunction onCloseFunc;
} Window;

int32_t Windowing_InitializeBackend();
const char **Windowing_GetRequiredInstanceExtensions(uint32_t *outExtensionsCount);
double Windowing_GetTime();
void Windowing_CollectEvents();
void Windowing_AwaitEvents();