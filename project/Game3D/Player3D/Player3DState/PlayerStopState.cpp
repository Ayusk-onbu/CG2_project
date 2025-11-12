#include "PlayerState.h"
#include "Player3D.h"
#include "ImGuiManager.h"

void PlayerStopState::Initialize() {

}

void PlayerStopState::Update() {

	bool isMove = false;
	if (InputManager::GetKey().PressKey(DIK_W)){isMove = true;}
	else if (InputManager::GetKey().PressKey(DIK_S)){isMove = true;}
	else if (InputManager::GetKey().PressKey(DIK_A)){isMove = true;}
	else if (InputManager::GetKey().PressKey(DIK_D)){isMove = true;}

	bool has_stamina = player_->GetStamina() > 0.0f;

	if (has_stamina == false) {
		// スタミナ切れ
		player_->ChangeState(new PlayerExhaustedState());
		return;
	}

	if (InputManager::IsJump()) {
		// ジャンプした
		player_->ChangeState(new PlayerJumpState());
		return;
	}
	
	if (isMove) {
		player_->ChangeState(new PlayerWalkState());
		return;
	}

	if (InputManager::IsDash()) {
		// 走る
		player_->ChangeState(new PlayerDashState());
		return;
	}

	if (InputManager::IsAttack()) {
		// 攻撃
		player_->ChangeState(new PlayerAttackState());
		return;
	}
	ImGuiManager::GetInstance()->Text("StopState");
}