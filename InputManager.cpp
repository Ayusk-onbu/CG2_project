#include "InputManager.h"
#include <cassert>

Key InputManager::key_;
Mouse InputManager::mouse_;

void InputManager::Initialize(WNDCLASS& wc, HWND hwnd) {

	HRESULT result;
#pragma region DirectInputの初期化
	result = DirectInput8Create(
		wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
#pragma endregion
	key_.Initialize(directInput, hwnd);
	mouse_.Initialize(directInput, hwnd);
}

void InputManager::Update() {
	key_.Update();
	mouse_.Update();
}