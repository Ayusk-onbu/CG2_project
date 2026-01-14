#include "Player3DCommand.h"
#include "InputManager.h"
#include "Player3D.h"

void Player3DMoveCommand::execute() {
	if (!player_)return;
	player_->Move().x = 
		std::sqrtf(InputManager::GetGamePad(0).GetLeftStickX() * InputManager::GetGamePad(0).GetLeftStickX()) > 0.15f ? InputManager::GetGamePad(0).GetLeftStickX() : 0.0f;
	player_->Move().z =
		std::sqrtf(InputManager::GetGamePad(0).GetLeftStickY() * InputManager::GetGamePad(0).GetLeftStickY()) > 0.15f ? InputManager::GetGamePad(0).GetLeftStickY() : 0.0f;
	if (InputManager::GetKey().PressKey(DIK_W)) {
		player_->Move().z += 1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_S)) {
		player_->Move().z += -1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_A)) {
		player_->Move().x -= 1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_D)) {
		player_->Move().x+= 1.0f;
	}
}

void Player3DJumpCommand::execute() {
	if (!player_)return;
	// ジャンプ処理を呼び出す
	player_->OnJumpInput();
}

void Player3DAttackCommand::execute() {
	if (!player_)return;
	// 攻撃処理を呼び出す
	player_->OnAttackInput();
}

void Player3DDashCommand::execute() {
	if (!player_)return;
	// ダッシュ処理を呼び出す
	player_->OnDashInput();
}

void Player3DEvasionCommand::execute() {
	if (!player_)return;
	// 回避処理を呼び出す
	player_->OnEvasionInput();
}