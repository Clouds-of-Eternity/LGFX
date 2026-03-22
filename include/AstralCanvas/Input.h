#pragma once
#include "AstralCanvas/Application.h"

DynamicFunction bool Input_IsKeyPressed(Keys key);
DynamicFunction bool Input_IsKeyDown(Keys key);
DynamicFunction bool Input_IsKeyReleased(Keys key);

DynamicFunction bool Input_IsMousePressed(MouseButtons mouseButton);
DynamicFunction bool Input_IsMouseDown(MouseButtons mouseButton);
DynamicFunction bool Input_IsMouseReleased(MouseButtons mouseButton);

DynamicFunction void Input_SimulateMousePress(MouseButtons mouseButton);
DynamicFunction void Input_SimulateMouseRelease(MouseButtons mouseButton);

DynamicFunction bool Input_ControllerIsButtonPressed(int32_t controllerIndex, ControllerButtons button);
DynamicFunction bool Input_ControllerIsButtonDown(int32_t controllerIndex, ControllerButtons button);
DynamicFunction bool Input_ControllerIsButtonReleased(int32_t controllerIndex, ControllerButtons button);

DynamicFunction float Input_ControllerGetR2DownAmount(uint32_t controllerIndex);
DynamicFunction float Input_ControllerGetL2DownAmount(uint32_t controllerIndex);

DynamicFunction Vec2 Input_ControllerGetLeftStickAxis(uint32_t controllerIndex);
DynamicFunction Vec2 Input_ControllerGetRightStickAxis(uint32_t controllerIndex);