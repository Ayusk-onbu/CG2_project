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