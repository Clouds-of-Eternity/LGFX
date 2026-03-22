#include "AstralCanvas/Input.h"
#include "./Application_c.h"
#include "./Window_c.h"

bool Input_IsKeyPressed(Keys key)
{
    return InputState_IsKeyPressed(&instance.currentWindow->windowInputState, key);
}
bool Input_IsKeyDown(Keys key)
{
    return InputState_IsKeyDown(&instance.currentWindow->windowInputState, key);
}
bool Input_IsKeyReleased(Keys key)
{
    return InputState_IsKeyReleased(&instance.currentWindow->windowInputState, key);
}
bool Input_IsMousePressed(MouseButtons mouseButton)
{
    return InputState_IsMousePressed(&instance.currentWindow->windowInputState, mouseButton);
}
bool Input_IsMouseDown(MouseButtons mouseButton)
{
    return InputState_IsMouseDown(&instance.currentWindow->windowInputState, mouseButton);
}
bool Input_IsMouseReleased(MouseButtons mouseButton)
{
    return InputState_IsMouseReleased(&instance.currentWindow->windowInputState, mouseButton);
}
void Input_SimulateMousePress(MouseButtons mouseButton)
{
    InputState_SimulateMousePress(&instance.currentWindow->windowInputState, mouseButton);
}
void Input_SimulateMouseRelease(MouseButtons mouseButton)
{
    InputState_SimulateMouseRelease(&instance.currentWindow->windowInputState, mouseButton);
}

bool Input_ControllerIsButtonPressed(int32_t controllerIndex, ControllerButtons button)
{
    return InputState_ControllerIsButtonPressed(&instance.currentWindow->windowInputState, controllerIndex, button);
}
bool Input_ControllerIsButtonDown(int32_t controllerIndex, ControllerButtons button)
{
    return InputState_ControllerIsButtonDown(&instance.currentWindow->windowInputState, controllerIndex, button);
}
bool Input_ControllerIsButtonReleased(int32_t controllerIndex, ControllerButtons button)
{
    return InputState_ControllerIsButtonReleased(&instance.currentWindow->windowInputState, controllerIndex, button);
}
float Input_ControllerGetR2DownAmount(uint32_t controllerIndex)
{
    return InputState_ControllerGetR2DownAmount(&instance.currentWindow->windowInputState, controllerIndex);
}
float Input_ControllerGetL2DownAmount(uint32_t controllerIndex)
{
    return InputState_ControllerGetL2DownAmount(&instance.currentWindow->windowInputState, controllerIndex);
}
Vec2 Input_ControllerGetLeftStickAxis(uint32_t controllerIndex)
{
    return InputState_ControllerGetLeftStickAxis(&instance.currentWindow->windowInputState, controllerIndex);
}
Vec2 Input_ControllerGetRightStickAxis(uint32_t controllerIndex)
{
    return InputState_ControllerGetRightStickAxis(&instance.currentWindow->windowInputState, controllerIndex);
}