#pragma once
#include "Linxc.h"
#include "denseset.hpp"
#include "Maths/Vec2.hpp"
#include "string.h"

#define CONTROLLER_BUTTONS 15

namespace AstralCanvas
{
    enum ControllerButtons
    {
        ControllerButton_A,
        ControllerButton_B,
        ControllerButton_X,
        ControllerButton_Y,
        ControllerButton_L1,
        ControllerButton_R1,
        ControllerButton_Back,
        ControllerButton_Start,
        ControllerButton_Center,
        ControllerButton_LeftAxis,
        ControllerButton_RightAxis,
        ControllerButton_DpadUp,
        ControllerButton_DpadRight,
        ControllerButton_DpadDown,
        ControllerButton_DpadLeft
    };
    enum Keys
    {
        Keys_Unknown = -1,
        Keys_Space = 32,
        Keys_Apostrophe = 39,
        Keys_Comma = 44,
        Keys_Minus = 45,
        Keys_Period = 46,
        Keys_Slash = 47,
        Keys_Number0 = 48,
        Keys_Number1 = 49,
        Keys_Number2 = 50,
        Keys_Number3 = 51,
        Keys_Number4 = 52,
        Keys_Number5 = 53,
        Keys_Number6 = 54,
        Keys_Number7 = 55,
        Keys_Number8 = 56,
        Keys_Number9 = 57,
        Keys_Semicolon = 59,
        Keys_Equal = 61,
        Keys_A = 65,
        Keys_B = 66,
        Keys_C = 67,
        Keys_D = 68,
        Keys_E = 69,
        Keys_F = 70,
        Keys_G = 71,
        Keys_H = 72,
        Keys_I = 73,
        Keys_J = 74,
        Keys_K = 75,
        Keys_L = 76,
        Keys_M = 77,
        Keys_N = 78,
        Keys_O = 79,
        Keys_P = 80,
        Keys_Q = 81,
        Keys_R = 82,
        Keys_S = 83,
        Keys_T = 84,
        Keys_U = 85,
        Keys_V = 86,
        Keys_W = 87,
        Keys_X = 88,
        Keys_Y = 89,
        Keys_Z = 90,
        Keys_LeftBracket = 91,
        Keys_BackSlash = 92,
        Keys_RightBracket = 93,
        Keys_GraveAccent = 96,
        Keys_World1 = 161,
        Keys_World2 = 162,
        Keys_Escape = 0x100,
        Keys_Enter = 257,
        Keys_Tab = 258,
        Keys_Backspace = 259,
        Keys_Insert = 260,
        Keys_Delete = 261,
        Keys_Right = 262,
        Keys_Left = 263,
        Keys_Down = 264,
        Keys_Up = 265,
        Keys_PageUp = 266,
        Keys_PageDown = 267,
        Keys_Home = 268,
        Keys_End = 269,
        Keys_CapsLock = 280,
        Keys_ScrollLock = 281,
        Keys_NumLock = 282,
        Keys_PrintScreen = 283,
        Keys_Pause = 284,
        Keys_F1 = 290,
        Keys_F2 = 291,
        Keys_F3 = 292,
        Keys_F4 = 293,
        Keys_F5 = 294,
        Keys_F6 = 295,
        Keys_F7 = 296,
        Keys_F8 = 297,
        Keys_F9 = 298,
        Keys_F10 = 299,
        Keys_F11 = 300,
        Keys_F12 = 301,
        Keys_F13 = 302,
        Keys_F14 = 303,
        Keys_F15 = 304,
        Keys_F16 = 305,
        Keys_F17 = 306,
        Keys_F18 = 307,
        Keys_F19 = 308,
        Keys_F20 = 309,
        Keys_F21 = 310,
        Keys_F22 = 311,
        Keys_F23 = 312,
        Keys_F24 = 313,
        Keys_F25 = 314,
        Keys_Keypad0 = 320,
        Keys_Keypad1 = 321,
        Keys_Keypad2 = 322,
        Keys_Keypad3 = 323,
        Keys_Keypad4 = 324,
        Keys_Keypad5 = 325,
        Keys_Keypad6 = 326,
        Keys_Keypad7 = 327,
        Keys_Keypad8 = 328,
        Keys_Keypad9 = 329,
        Keys_KeypadDecimal = 330,
        Keys_KeypadDivide = 331,
        Keys_KeypadMultiply = 332,
        Keys_KeypadSubtract = 333,
        Keys_KeypadAdd = 334,
        Keys_KeypadEnter = 335,
        Keys_KeypadEqual = 336,
        Keys_ShiftLeft = 340,
        Keys_ControlLeft = 341,
        Keys_AltLeft = 342,
        Keys_SuperLeft = 343,
        Keys_ShiftRight = 344,
        Keys_ControlRight = 345,
        Keys_AltRight = 346,
        Keys_SuperRight = 347,
        Keys_Menu = 348,
        Keys_LastKey = 348
    };
    enum MouseButtons
    {
        MouseButton_Left,
        MouseButton_Right,
        MouseButton_Middle,
        MouseButton_Other
    };
    enum KeyState
    {
        KeyState_None,
        KeyState_Released,
        KeyState_Pressed,
        KeyState_Repeat
    };
    struct ControllerState
    {
        /// Whether this controller is connected
        bool connected;
        /// The position of the left stick (-1, -1) to (1, 1)
        Maths::Vec2 leftStickAxis;
        /// The position of the right stick (-1, -1) to (1, 1)
        Maths::Vec2 rightStickAxis;
        /// How hard the user has pressed the L2 trigger button
        float L2DownAmount;
        /// How hard the user has pressed the R2 trigger button
        float R2DownAmount;
        /// Whether each button on the controller is pressed
        bool buttonStates[CONTROLLER_BUTTONS];

        inline void CopyTo(ControllerState *other)
        {
            other->connected = connected;
            other->leftStickAxis = leftStickAxis;
            other->rightStickAxis = rightStickAxis;
            other->L2DownAmount = L2DownAmount;
            other->R2DownAmount = R2DownAmount;
            memcpy(other->buttonStates, buttonStates, sizeof(bool) * CONTROLLER_BUTTONS);
        }
    };
    struct KeyStateStatus
    {
        bool status;
        KeyState perFrameState;
    };
    struct InputState
    {
        ControllerState oldControllerStates[4];
        ControllerState controllerStates[4];
        Maths::Vec2 mousePosition;
        Maths::Vec2 worldMousePosition;
        Maths::Vec2 scroll;
        
        collections::denseset<KeyStateStatus> keyStatuses;
        collections::denseset<KeyStateStatus> mouseStatuses;

        u32 textInputCharacter;

        inline InputState()
        {
            textInputCharacter = 0;
            oldControllerStates[0] = {};
            oldControllerStates[1] = {};
            oldControllerStates[2] = {};
            oldControllerStates[3] = {};

            controllerStates[0] = {};
            controllerStates[1] = {};
            controllerStates[2] = {};
            controllerStates[3] = {};

            mousePosition = Maths::Vec2(0.0f, 0.0f);
            worldMousePosition = Maths::Vec2(0.0f, 0.0f);
            scroll = Maths::Vec2(0.0f, 0.0f);

            keyStatuses = collections::denseset<KeyStateStatus>();
            mouseStatuses = collections::denseset<KeyStateStatus>();
        }
        inline InputState(IAllocator allocator)
        {
            textInputCharacter = 0;
            oldControllerStates[0] = {};
            oldControllerStates[1] = {};
            oldControllerStates[2] = {};
            oldControllerStates[3] = {};

            controllerStates[0] = {};
            controllerStates[1] = {};
            controllerStates[2] = {};
            controllerStates[3] = {};

            mousePosition = Maths::Vec2(0.0f, 0.0f);
            worldMousePosition = Maths::Vec2(0.0f, 0.0f);
            scroll = Maths::Vec2(0.0f, 0.0f);

            keyStatuses = collections::denseset<KeyStateStatus>(allocator);
            mouseStatuses = collections::denseset<KeyStateStatus>(allocator);
        }
        inline bool IsKeyDown(Keys key)
        {
            return keyStatuses.Get((usize)key) != NULL && keyStatuses.Get((usize)key)->status;
        }
        inline bool IsKeyPressed(Keys key)
        {
            return keyStatuses.Get((usize)key) != NULL && keyStatuses.Get((usize)key)->perFrameState == KeyState_Pressed;
        }
        inline bool IsKeyReleased(Keys key)
        {
            return keyStatuses.Get((usize)key) != NULL && keyStatuses.Get((usize)key)->perFrameState == KeyState_Released;
        }

        inline bool IsMouseDown(MouseButtons button)
        {
            return mouseStatuses.Get((usize)button) != NULL && mouseStatuses.Get((usize)button)->status;
        }
        inline bool IsMousePressed(MouseButtons button)
        {
            return mouseStatuses.Get((usize)button) != NULL && mouseStatuses.Get((usize)button)->perFrameState == KeyState_Pressed;
        }
        inline bool IsMouseReleased(MouseButtons button)
        {
            return mouseStatuses.Get((usize)button) != NULL && mouseStatuses.Get((usize)button)->perFrameState == KeyState_Released;
        }
        inline void SimulateMousePress(MouseButtons button)
        {
            mouseStatuses.Insert((usize)button, {true, KeyState_Pressed});
        }
        inline void SimulateMouseRelease(MouseButtons button)
        {
            mouseStatuses.Insert((usize)button, {false, KeyState_Released});
        }
        bool IsControllerConnected(usize index);

        ControllerState GetControllerState(usize index);

        void SetMousePosition(void *windowPtr, Maths::Vec2 position);

        inline void ResetPerFrameInputStates()
        {
            textInputCharacter = '\0';
            scroll = Maths::Vec2(0.0f);
            for (usize i = 0; i < keyStatuses.capacity; i++)
            {
                keyStatuses.ptr[i].perFrameState = KeyState_None;
            }
            for (usize i = 0; i < mouseStatuses.capacity; i++)
            {
                mouseStatuses.ptr[i].perFrameState = KeyState_None;
            }
            for (usize i = 0; i < 4; i++)
            {
                oldControllerStates[i] = controllerStates[i];
                controllerStates[i] = GetControllerState(i);
            }
        }

        /// Called when the window is minimized or any other reason that may cause the window to stop recording key presses
        inline void ClearAllInputStates()
        {
            ResetPerFrameInputStates();
            for (int i = 0; i < keyStatuses.capacity; i++)
            {
                keyStatuses.ptr[i].status = false;
            }
            for (int i = 0; i < mouseStatuses.capacity; i++)
            {
                mouseStatuses.ptr[i].status = false;
            }
        }
    };
}