#include "InputManager.h"
#include "InputManager.h"
#include <cassert>

Key InputManager::key_;
Mouse InputManager::mouse_;
GamePad InputManager::gamePad_[4]; // 最大4つのゲームパッドをサポート
Microsoft::WRL::ComPtr<IDirectInput8> InputManager::directInput_;

void InputManager::Initialize(WNDCLASS& wc, HWND hwnd) {

	HRESULT result;
	// DirectInputの初期化
	result = DirectInput8Create(
		wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	key_.Initialize(directInput_, hwnd);
	mouse_.Initialize(directInput_, hwnd);
}

void InputManager::Update() {

	// ---- --   ------- ---------  -------- //
	// Key, Mouse, GamePadの更新             //
	//--- - --   ------- ---------  -------- //

	key_.Update();
	mouse_.Update();
	for (int i = 0; i < 4; ++i) {
		gamePad_[i].Update();
	}
}

bool InputManager::IsJump() {
	bool isJump = false;
	if (GetKey().PressedKey(DIK_SPACE)) {
		isJump = true;
	}
	if (GetGamePad(0).IsPressed(XINPUT_GAMEPAD_A)) {
		isJump = true;
	}

	return isJump;
}

bool InputManager::IsDash() {
	bool isDash = false;
	if (GetKey().PressKey(DIK_LSHIFT)) {
		isDash = true;
	}
	if (0) {

	}
	return isDash;
}

bool InputManager::IsAttack() {
	bool isAttack = false;
	if (GetKey().PressKey(DIK_J)) {
		isAttack = true;
	}
	if (0) {

	}
	return isAttack;
}