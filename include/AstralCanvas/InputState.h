#pragma once
#include "Maths/Vectors.h"
#include "List.h"
#include "Array.h"

#define CONTROLLER_BUTTONS 17

typedef enum ControllerButtons
{
    ControllerButton_Unknown = -1,
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
    ControllerButton_DpadLeft,
    ControllerButton_L2,
    ControllerButton_R2,
    ControllerButton_MAX = 0x7FFFFFFF
} ControllerButtons;

typedef enum Keys
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
    Keys_LastKey = 348,
    Keys_MAX = 0x7FFFFFFF
} Keys;

typedef enum MouseButtons
{
    MouseButton_Left,
    MouseButton_Right,
    MouseButton_Middle,
    MouseButton_Other,
    MouseButton_LastMouseButton,
    MouseButtons_MAX = 0x7FFFFFFF
} MouseButtons;
typedef enum KeyState
{
    KeyState_None,
    KeyState_Released,
    KeyState_Pressed,
    KeyState_Repeat,
    KeyState_MAX = 0x7FFFFFFF
} KeyState;

typedef struct ControllerState
{
    Vec2 leftStickAxis;
    Vec2 rightStickAxis;
    float L2DownAmount;
    float R2DownAmount;
    bool connected;
    bool buttonStates[CONTROLLER_BUTTONS];
} ControllerState;

typedef struct KeyStateStatus
{
    bool status;
    KeyState perFrameState;
} KeyStateStatus;

typedef struct InputState
{
    /// @brief The previous button states of up to four connected controllers.
    ControllerState oldControllerStates[4];
    /// @brief The current button states of up to four connected controllers.
    ControllerState controllerStates[4];

    /// @brief The mouse position relative to the window or canvas.
    Vec2 mousePosition;
    /// @brief The mouse position in the contents of the window itself. This value is user-defined
    /// and is not mandatory. It is not used anywhere in Astral.Canvas itself.
    Vec2 worldMousePosition;
    /// @brief The difference in mouse scroll between the time that this InputState was retrieved
    /// and the previous.
    Vec2 scroll;

    /// @brief Array of KeyStateStatus representing the states of the key control inputs at the time
    /// of which this InputState was retrieved. These keys would correspond to what one would
    /// expect on a QWERTY keyboard, and should be used for control input. Text input is recorded
    /// through textInputCharacters instead.
    KeyStateStatus *keyStatuses;
    /// @brief Array of KeyStateStatus representing the states of the mouse buttons at the time
    /// of which this InputState was retrieved.
    KeyStateStatus *mouseStatuses;
    /// @brief List of uint32_t representing the character codes inputted from the relevant
    /// input device since last update.
    List textInputCharacters;
} InputState;

/// @brief Creates and returns a new zero-initialized InputState instance.
DynamicFunction InputState InputState_Empty();
/// @brief Creates a InputState instance with normal defaults.
DynamicFunction InputState InputState_Create();
/// @brief Frees all allocated heap memory within the InputState
DynamicFunction void InputState_Deinit(InputState *self);
/// @brief Queries an InputState on whether the given key has just been pressed this frame.
DynamicFunction bool InputState_IsKeyPressed(const InputState *self, Keys key);
/// @brief Queries an InputState on whether the given key is being held down.
DynamicFunction bool InputState_IsKeyDown(const InputState *self, Keys key);
/// @brief Queries an InputState on whether the given key has just been released this frame.
DynamicFunction bool InputState_IsKeyReleased(const InputState *self, Keys key);
/// @brief Queries an InputState on whether the given mouse button has just been pressed this frame.
DynamicFunction bool InputState_IsMousePressed(const InputState *self, MouseButtons button);
/// @brief Queries an InputState on whether the given mouse button is being held down.
DynamicFunction bool InputState_IsMouseDown(const InputState *self, MouseButtons button);
/// @brief Queries an InputState on whether the given mouse button has just been released this frame.
DynamicFunction bool InputState_IsMouseReleased(const InputState *self, MouseButtons button);
/// @brief Sets the appropriate variables on the given InputState as if the specified
/// mouse button had just been pressed. This only affects applications reading from the InputState,
/// and does not actually simulate a mouse press for the operating system itself.
DynamicFunction void InputState_SimulateMousePress(InputState *self, MouseButtons button);
/// @brief Unsets the appropriate variables on the given InputState as if the specified
/// mouse button had just been released. This only affects applications reading from the InputState,
/// and does not actually simulate a mouse press for the operating system itself.
DynamicFunction void InputState_SimulateMouseRelease(InputState *self, MouseButtons button);

DynamicFunction bool Input_ControllerIsConnected(uint32_t controllerIndex);
DynamicFunction ControllerState Input_GetControllerState(uint32_t controllerIndex);
DynamicFunction bool InputState_ControllerIsButtonPressed(const InputState *self, int32_t controllerIndex, ControllerButtons button);
DynamicFunction bool InputState_ControllerIsButtonDown(const InputState *self, int32_t controllerIndex, ControllerButtons button);
DynamicFunction bool InputState_ControllerIsButtonReleased(const InputState *self, int32_t controllerIndex, ControllerButtons button);

DynamicFunction float InputState_ControllerGetL2DownAmount(const InputState *self, uint32_t controllerIndex);
DynamicFunction float InputState_ControllerGetR2DownAmount(const InputState *self, uint32_t controllerIndex);
DynamicFunction Vec2 InputState_ControllerGetLeftStickAxis(const InputState *self, uint32_t controllerIndex);
DynamicFunction Vec2 InputState_ControllerGetRightStickAxis(const InputState *self, uint32_t controllerIndex);

///@brief Resets the cache of key, mouse and controller inputs, as well as the 
/// internal textInputCharacters store. This is called at the end of each (active) frame.
DynamicFunction void InputState_ResetPerFrameStates(InputState *self);
///@brief Calls InputState_ResetPerFrameStates and also resets key and mouse statuses.
/// Called when the window is minimized or any other reason that may
/// cause the window to stop recording key presses and thus send erroneous signals to the
/// receiving application.
DynamicFunction void InputState_ResetAllInputStates(InputState *self);

DynamicFunction ControllerState ControllerState_Empty();
DynamicFunction void ControllerState_CopyTo(const ControllerState *self, ControllerState *other);