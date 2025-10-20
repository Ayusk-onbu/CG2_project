#include "GamePad.h"

#pragma comment(lib, "Xinput.lib")

GamePad::GamePad(int index)
    : controllerIndex_(index), isConnected_(false)
{
    ZeroMemory(&state_, sizeof(XINPUT_STATE));
}

void GamePad::Update() {
    ZeroMemory(&state_, sizeof(XINPUT_STATE));
    DWORD result = XInputGetState(controllerIndex_, &state_);
    isConnected_ = (result == ERROR_SUCCESS);
}

bool GamePad::IsConnected() const {
    return isConnected_;
}

bool GamePad::IsPressed(WORD button) {
    if (!isConnected_) return false;
    return (state_.Gamepad.wButtons & button) != 0;
}

BYTE GamePad::GetLeftTrigger() const {
    return isConnected_ ? state_.Gamepad.bLeftTrigger : 0;
}

SHORT GamePad::GetLeftStickX() const {
    return isConnected_ ? state_.Gamepad.sThumbLX : 0;
}

SHORT GamePad::GetLeftStickY() const {
    return isConnected_ ? state_.Gamepad.sThumbLY : 0;
}

SHORT GamePad::GetRightStickX() const {
    return isConnected_ ? state_.Gamepad.sThumbRX : 0;
}
SHORT GamePad::GetRightStickY() const {
    return isConnected_ ? state_.Gamepad.sThumbRY : 0;
}
BYTE GamePad::GetRightTrigger() const {
    return isConnected_ ? state_.Gamepad.bRightTrigger : 0;
}
