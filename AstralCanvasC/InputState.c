#include "AstralCanvas/InputState.h"

ControllerState ControllerState_Empty()
{
    const ControllerState result = {};
    return result;
}
void ControllerState_CopyTo(const ControllerState *self, ControllerState *other)
{
    other->connected = self->connected;
    other->leftStickAxis = self->leftStickAxis;
    other->rightStickAxis = self->rightStickAxis;
    other->L2DownAmount = self->L2DownAmount;
    other->R2DownAmount = self->R2DownAmount;
    memcpy(other->buttonStates, self->buttonStates, sizeof(bool) * CONTROLLER_BUTTONS);
}
InputState InputState_Empty()
{
    const InputState result = {};
    return result;
}
InputState InputState_Create()
{
    const ControllerState emptyControllerState = {};
    InputState result;

    result.textInputCharacters = List_Create(GetCAllocator(), sizeof(uint32_t));
    result.oldControllerStates[0] = emptyControllerState;
    result.oldControllerStates[1] = emptyControllerState;
    result.oldControllerStates[2] = emptyControllerState;
    result.oldControllerStates[3] = emptyControllerState;

    result.controllerStates[0] = emptyControllerState;
    result.controllerStates[1] = emptyControllerState;
    result.controllerStates[2] = emptyControllerState;
    result.controllerStates[3] = emptyControllerState;

    result.mousePosition = CreateVec2(0.0f, 0.0f);
    result.worldMousePosition = CreateVec2(0.0f, 0.0f);
    result.scroll = CreateVec2(0.0f, 0.0f);

    result.keyStatuses = (KeyStateStatus *)malloc(sizeof(KeyStateStatus) * Keys_LastKey);
    result.mouseStatuses = (KeyStateStatus *)malloc(sizeof(KeyStateStatus) * MouseButton_LastMouseButton);

    return result;
}
void InputState_Deinit(InputState *self)
{
    List_Deinit(&self->textInputCharacters);
    free(self->keyStatuses);
    free(self->mouseStatuses);
}

bool InputState_IsKeyPressed(const InputState *self, Keys key)
{
    #ifdef DEBUG
    if (key >= Keys_LastKey)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->keyStatuses[(uint32_t)key];
    return status->status && status->perFrameState == KeyState_Pressed;
}
bool InputState_IsKeyDown(const InputState *self, Keys key)
{
    #ifdef DEBUG
    if (key >= Keys_LastKey)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->keyStatuses[(uint32_t)key];
    return status->status;
}
bool InputState_IsKeyReleased(const InputState *self, Keys key)
{
    #ifdef DEBUG
    if (key >= Keys_LastKey)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->keyStatuses[(uint32_t)key];
    return status->status && status->perFrameState == KeyState_Released;
}

bool InputState_IsMousePressed(const InputState *self, MouseButtons button)
{
    #ifdef DEBUG
    if (button >= MouseButton_LastMouseButton)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->mouseStatuses[(uint32_t)button];
    return status->status && status->perFrameState == KeyState_Pressed;
}
bool InputState_IsMouseDown(const InputState *self, MouseButtons button)
{
    #ifdef DEBUG
    if (button >= MouseButton_LastMouseButton)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->mouseStatuses[(uint32_t)button];
    return status->status;
}
bool InputState_IsMouseReleased(const InputState *self, MouseButtons button)
{
    #ifdef DEBUG
    if (button >= MouseButton_LastMouseButton)
    {
        return false;
    }
    #endif
    const KeyStateStatus *status = &self->mouseStatuses[(uint32_t)button];
    return status->status && status->perFrameState == KeyState_Released;
}

void InputState_SimulateMousePress(InputState *self, MouseButtons button)
{
    self->mouseStatuses[(uint32_t)button].perFrameState = KeyState_Pressed;
    self->mouseStatuses[(uint32_t)button].status = true;
}
void InputState_SimulateMouseRelease(InputState *self, MouseButtons button)
{
    self->mouseStatuses[(uint32_t)button].perFrameState = KeyState_Released;
    self->mouseStatuses[(uint32_t)button].status = false;
}

bool InputState_ControllerIsButtonPressed(const InputState *self, int32_t controllerIndex, ControllerButtons button)
{
    const ControllerState *states = self->controllerStates;
    const ControllerState *oldStates = self->oldControllerStates;

    if (button == ControllerButton_L2)
    {
        if (controllerIndex == -1)
        {
            for (uint32_t i = 0; i < 4; i++)
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
            for (uint32_t i = 0; i < 4; i++)
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
        for (uint32_t i = 0; i < 4; i++)
        {
            if (states[i].connected && states[i].buttonStates[(size_t)button] && !oldStates[i].buttonStates[(size_t)button])
            {
                return true;
            }
        }
        return false;
    }
    else
    {
        return states[controllerIndex].connected && 
        states[controllerIndex].buttonStates[(size_t)button]
        && !oldStates[controllerIndex].buttonStates[(size_t)button];
    }
}
bool InputState_ControllerIsButtonDown(const InputState *self, int32_t controllerIndex, ControllerButtons button)
{
    const ControllerState *states = self->controllerStates;
    if (button == ControllerButton_L2)
    {
        if (controllerIndex == -1)
        {
            for (uint32_t i = 0; i < 4; i++)
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
            for (uint32_t i = 0; i < 4; i++)
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
        for (uint32_t i = 0; i < 4; i++)
        {
            if (states[i].connected && states[i].buttonStates[(size_t)button])
            {
                return true;
            }
        }
        return false;
    }
    else return states[controllerIndex].connected && states[controllerIndex].buttonStates[(size_t)button];
}
bool InputState_ControllerIsButtonReleased(const InputState *self, int32_t controllerIndex, ControllerButtons button)
{
    ControllerState *states = self->controllerStates;
    ControllerState *oldStates = self->oldControllerStates;

    if (button == ControllerButton_L2)
    {
        if (controllerIndex == -1)
        {
            for (uint32_t i = 0; i < 4; i++)
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
            for (uint32_t i = 0; i < 4; i++)
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
        for (uint32_t i = 0; i < 4; i++)
        {
            if (states[i].connected && !states[i].buttonStates[(size_t)button] && oldStates[i].buttonStates[(size_t)button])
            {
                return true;
            }
        }
        return false;
    }
    else
    {
        return states[controllerIndex].connected && 
        !states[controllerIndex].buttonStates[(size_t)button]
        && oldStates[controllerIndex].buttonStates[(size_t)button];
    }
}
float InputState_ControllerGetL2DownAmount(const InputState *self, uint32_t controllerIndex)
{
    if (controllerIndex >= 4)
    {
        return 0.0f;
    }
    return self->controllerStates[controllerIndex].L2DownAmount;
}
float InputState_ControllerGetR2DownAmount(const InputState *self, uint32_t controllerIndex)
{
    if (controllerIndex >= 4)
    {
        return 0.0f;
    }
    return self->controllerStates[controllerIndex].R2DownAmount;
}
Vec2 InputState_ControllerGetLeftStickAxis(const InputState *self, uint32_t controllerIndex)
{
    if (controllerIndex >= 4)
    {
        return CreateVec2(0.0f, 0.0f);
    }
    return self->controllerStates[controllerIndex].leftStickAxis;
}
Vec2 InputState_ControllerGetRightStickAxis(const InputState *self, uint32_t controllerIndex)
{
    if (controllerIndex >= 4)
    {
        return CreateVec2(0.0f, 0.0f);
    }
    return self->controllerStates[controllerIndex].rightStickAxis;
}

void InputState_ResetPerFrameStates(InputState *self)
{
    List_Clear(&self->textInputCharacters);
    self->scroll = CreateVec2(0.0f, 0.0f);
    for (size_t i = 0; i < (size_t)Keys_LastKey; i++)
    {
        self->keyStatuses[i].perFrameState = KeyState_None;
    }
    for (size_t i = 0; i < (size_t)MouseButton_LastMouseButton; i++)
    {
        self->mouseStatuses[i].perFrameState = KeyState_None;
    }
    for (size_t i = 0; i < 4; i++)
    {
        self->oldControllerStates[i] = self->controllerStates[i];
        self->controllerStates[i] = Input_GetControllerState(i);
    }
}
void InputState_ResetAllInputStates(InputState *self)
{
    List_Clear(&self->textInputCharacters);
    self->scroll = CreateVec2(0.0f, 0.0f);
    for (size_t i = 0; i < (size_t)Keys_LastKey; i++)
    {
        self->keyStatuses[i].perFrameState = KeyState_None;
        self->keyStatuses[i].status = false;
    }
    for (size_t i = 0; i < (size_t)MouseButton_LastMouseButton; i++)
    {
        self->mouseStatuses[i].perFrameState = KeyState_None;
        self->mouseStatuses[i].status = false;
    }
    for (size_t i = 0; i < 4; i++)
    {
        self->oldControllerStates[i] = self->controllerStates[i];
        self->controllerStates[i] = Input_GetControllerState(i);
    }
}