#include "AstralCanvas/InputState.h"
#include "GLFW/glfw3.h"

bool Input_ControllerIsConnected(uint32_t controllerIndex)
{
    return glfwJoystickPresent((int)controllerIndex);
}

ControllerState Input_GetControllerState(uint32_t controllerIndex)
{
    ControllerState result = {};

    GLFWgamepadstate state;
    if (glfwGetGamepadState((int)controllerIndex, &state) != GLFW_FALSE)
    {
        result.connected = true;
        result.leftStickAxis = CreateVec2(state.axes[0], state.axes[1]);
        result.rightStickAxis = CreateVec2(state.axes[2], state.axes[3]);
        result.L2DownAmount = state.axes[4];
        result.R2DownAmount = state.axes[5];

        for (size_t i = 0; i < 15; i++)
        {
            result.buttonStates[i] = state.buttons[i] == GLFW_PRESS;
        }
    }
    return result;
}