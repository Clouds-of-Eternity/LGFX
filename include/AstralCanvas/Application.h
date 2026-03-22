#pragma once
#include "AstralCanvas/Window.h"
#include "lgfx/lgfx.h"

typedef struct Application Application;

def_delegate(ApplicationUpdateFunction, void, float);
def_delegate(ApplicationDrawFunction, void, float, Window *);
def_delegate(ApplicationVoidFunction, void);

DynamicFunction Application *AstralCanvas_GetApplication();
DynamicFunction LGFXDevice AstralCanvas_GetGraphicsDevice();
DynamicFunction LGFXInstance AstralCanvas_GetGraphicsInstance();

DynamicFunction Window *AstralCanvas_GetCurrentWindow();
DynamicFunction LGFXSwapchain AstralCanvas_GetCurrentSwapchain();
DynamicFunction Window *AstralCanvas_GetWindow(size_t index);
DynamicFunction LGFXSwapchain AstralCanvas_GetWindowSwapchain(size_t index);

DynamicFunction Window *Application_GetCurrentWindow(const Application *self);
DynamicFunction LGFXSwapchain Application_GetCurrentSwapchain(const Application *self);
DynamicFunction Window *Application_GetWindow(Application *self, size_t index);
DynamicFunction LGFXSwapchain Application_GetWindowSwapchain(Application *self, size_t index);

DynamicFunction Application *AstralCanvas_Init(const char *appName, const char *engineName, uint32_t appVersion, uint32_t engineVersion, bool noWindow);
DynamicFunction void AstralCanvas_Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction fixedUpdateFunc, ApplicationDrawFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationVoidFunction initFunc, ApplicationVoidFunction deinitFunc);
DynamicFunction void AstralCanvas_ResetDeltaTimer();

DynamicFunction double AstralCanvas_GetElapsedTime();

DynamicFunction void AstralCanvas_SetClipboardText(const char *value);
DynamicFunction const char *AstralCanvas_GetClipboardText();