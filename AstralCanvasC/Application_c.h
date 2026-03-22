#pragma once
#include "AstralCanvas/Application.h"

typedef struct Application
{
    List windows;
    Window *currentWindow;

    LGFXInstance instance;
    LGFXDevice device;

    bool alwaysUpdate;
    bool shouldShutdown;

    string appName;
    string engineName;
    u32 appVersion;
    u32 engineVersion;
    float startTime;
    float endTime;
    float updateTimer;
    float fixedUpdateTimer;
    bool shouldResetDeltaTimer;

    float framesPerSecond;
    float fixedTimeStep;
    float timeScale;
} Application;

extern Application instance;