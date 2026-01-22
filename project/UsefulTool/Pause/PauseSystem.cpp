#include "PauseSystem.h"
#include "InputManager.h"

std::unique_ptr<PauseSystem>PauseSystem::instance_ = nullptr;

void PauseSystem::Initialize() {

}

bool PauseSystem::Update(bool canPause) {
	// 更新処理
	// [ 入力処理 ]
	bool inputPause = false;
	// [ キーボード ]
	if (InputManager::GetKey().PressedKey(DIK_ESCAPE)) {
		inputPause = true;
	}
	// [ ゲームパッド ]
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_START)) {
		inputPause = true;
	}
	// [ ポーズ中ではない ]
	if (pause_ == false) {
		// [ ポーズ可能 && ポーズボタンを押した ]
		if (canPause && inputPause) {
			// [ ポーズ ]
			pause_ = true;
		}
	}
	// [ ポーズ中 ]
	else if (pause_ == true) {
		// [ ポーズボタンを押した ] 
		if (inputPause) {
			// [ ポーズ解除 ]
			pause_ = false;
		}
	}

	// [ ポーズ中か否か ] 
	return pause_;
}

void PauseSystem::Draw() {

}