#pragma once
#include "lgfx-astral/Application.hpp"
#include "InputState.hpp"
#include "Maths/Vec2.hpp"

namespace AstralCanvas
{
    inline AstralCanvas::Window *Input_GetWindow()
    {
        return AstralCanvas::applicationInstance.currentWindow;
    }
    inline bool Input_IsKeyDown(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyDown(key);
    }
    inline bool Input_IsKeyPressed(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyPressed(key);
    }
    inline bool Input_IsKeyReleased(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyReleased(key);
    }

    inline bool Input_IsMouseDown(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseDown(button);
    }
    inline bool Input_IsMousePressed(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMousePressed(button);
    }
    inline bool Input_IsMouseReleased(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseReleased(button);
    }

    inline void Input_SimulateMousePress(const MouseButtons button)
    {
        Input_GetWindow()->windowInputState.SimulateMousePress(button);
    }
    inline void Input_SimulateMouseRelease(const MouseButtons button)
    {
        Input_GetWindow()->windowInputState.SimulateMouseRelease(button);
    }

    inline bool Input_ControllerIsButtonDown(const i32 controllerIndex, const ControllerButtons button)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;

        if (button == ControllerButton_L2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].L2DownAmount > 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].L2DownAmount > 0.1f;
        }
        if (button == ControllerButton_R2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].R2DownAmount > 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].R2DownAmount > 0.1f;
        }
        if (controllerIndex == -1)
        {
            for (u32 i = 0; i < 4; i++)
            {
                if (states[i].connected && states[i].buttonStates[(usize)button])
                {
                    return true;
                }
            }
            return false;
        }
        else return states[controllerIndex].connected && states[controllerIndex].buttonStates[(usize)button];
    }
    inline bool Input_ControllerIsButtonPress(const i32 controllerIndex, const ControllerButtons button)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        AstralCanvas::ControllerState *oldStates = Input_GetWindow()->windowInputState.oldControllerStates;

        if (button == ControllerButton_L2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].L2DownAmount > 0.1f && oldStates[i].L2DownAmount <= 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].L2DownAmount > 0.1f && oldStates[controllerIndex].L2DownAmount <= 0.1f;
        }
        if (button == ControllerButton_R2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].R2DownAmount > 0.1f && oldStates[i].R2DownAmount <= 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].R2DownAmount > 0.1f && oldStates[controllerIndex].R2DownAmount <= 0.1f;
        }
        if (controllerIndex == -1)
        {
            for (u32 i = 0; i < 4; i++)
            {
                if (states[i].connected && states[i].buttonStates[(usize)button] && !oldStates[i].buttonStates[(usize)button])
                {
                    return true;
                }
            }
            return false;
        }
        else
        {
            return states[controllerIndex].connected && 
            states[controllerIndex].buttonStates[(usize)button]
            && !oldStates[controllerIndex].buttonStates[(usize)button];
        }
    }
    inline bool Input_ControllerIsButtonRelease(const i32 controllerIndex, const ControllerButtons button)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        AstralCanvas::ControllerState *oldStates = Input_GetWindow()->windowInputState.oldControllerStates;

        if (button == ControllerButton_L2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].L2DownAmount <= 0.1f && oldStates[i].L2DownAmount > 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].L2DownAmount <= 0.1f && oldStates[controllerIndex].L2DownAmount > 0.1f;
        }
        if (button == ControllerButton_R2)
        {
            if (controllerIndex == -1)
            {
                for (u32 i = 0; i < 4; i++)
                {
                    if (states[i].connected && states[i].R2DownAmount <= 0.1f && oldStates[i].R2DownAmount > 0.1f)
                    {
                        return true;
                    }
                }
                return false;
            }
            return states[controllerIndex].connected && states[controllerIndex].R2DownAmount <= 0.1f && oldStates[controllerIndex].R2DownAmount > 0.1f;
        }
        if (controllerIndex == -1)
        {
            for (u32 i = 0; i < 4; i++)
            {
                if (states[i].connected && !states[i].buttonStates[(usize)button] && oldStates[i].buttonStates[(usize)button])
                {
                    return true;
                }
            }
            return false;
        }
        else
        {
            return states[controllerIndex].connected && 
            !states[controllerIndex].buttonStates[(usize)button]
            && oldStates[controllerIndex].buttonStates[(usize)button];
        }
    }

    inline bool Input_ControllerIsR2Down(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        return states[controllerIndex].connected && states[controllerIndex].R2DownAmount > 0.1f;
    }
    inline float Input_ControllerGetR2DownAmount(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        return states[controllerIndex].connected && states[controllerIndex].R2DownAmount;
    }

    inline bool Input_ControllerIsL2Down(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        return states[controllerIndex].connected && states[controllerIndex].L2DownAmount > 0.1f;
    }
    inline float Input_ControllerGetL2DownAmount(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        return states[controllerIndex].connected && states[controllerIndex].L2DownAmount;
    }

    inline bool Input_ControllerIsConnected(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].connected;
    }
    inline Maths::Vec2 Input_GetLeftStickAxis(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        if (!states[controllerIndex].connected)
        {
            return Maths::Vec2();
        }
        return states[controllerIndex].leftStickAxis;
    }
    inline Maths::Vec2 Input_GetRightStickAxis(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        if (!states[controllerIndex].connected)
        {
            return Maths::Vec2();
        }
        return states[controllerIndex].rightStickAxis;
    }

    inline bool Input_ControllerIsL2Pressed(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        
        return states[controllerIndex].connected && states[controllerIndex].L2DownAmount > 0.1f
        && Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].L2DownAmount <= 0.1f;
    }
    inline bool Input_ControllerIsR2Pressed(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        
        return states[controllerIndex].connected && states[controllerIndex].R2DownAmount > 0.1f
        && Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].R2DownAmount <= 0.1f;
    }
    inline bool Input_ControllerIsL2Released(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        
        return states[controllerIndex].connected && states[controllerIndex].L2DownAmount <= 0.1f
        && Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].L2DownAmount > 0.1f;
    }
    inline bool Input_ControllerIsR2Released(const u32 controllerIndex)
    {
        AstralCanvas::ControllerState *states = Input_GetWindow()->windowInputState.controllerStates;
        
        return states[controllerIndex].connected && states[controllerIndex].R2DownAmount <= 0.1f
        && Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].R2DownAmount > 0.1f;
    }
}