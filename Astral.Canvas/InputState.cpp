#include "GLFW/glfw3.h"
#include "lgfx-astral/InputState.hpp"

namespace AstralCanvas
{
    bool InputState::IsControllerConnected(usize index)
    {
        return glfwJoystickPresent(index);
    }
    ControllerState InputState::GetControllerState(usize index)
    {
        ControllerState result{};

        GLFWgamepadstate state;
        if (glfwGetGamepadState(index, &state) != GLFW_FALSE)
        {
            result.connected = true;
            result.leftStickAxis = Maths::Vec2(state.axes[0], state.axes[1]);
            result.rightStickAxis = Maths::Vec2(state.axes[2], state.axes[3]);
            result.L2DownAmount = state.axes[4];
            result.R2DownAmount = state.axes[5];

            for (usize i = 0; i < 15; i++)
            {
                result.buttonStates[i] = state.buttons[i] == GLFW_PRESS;
            }
        }
        return result;
    }
    void InputState::SetMousePosition(void *windowPtr, Maths::Vec2 position)
    {
        glfwSetCursorPos((GLFWwindow*)windowPtr, (double)position.X, (double)position.Y);
    }
}