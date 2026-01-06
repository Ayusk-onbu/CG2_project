#include "PlayerState.h"
#include "Player3D.h"
#include "ImGuiManager.h"

void PlayerStopState::Initialize() {
	player_->BlockMove();
	// 速度を元に戻す(戻っていなかった時の対策)
	player_->SetSpeedMultiplier(1.0f);
}

void PlayerStopState::Update() {

	bool isMove = player_->IsMoving();
	bool has_stamina = player_->GetStamina() > 0.0f;

	if (has_stamina == false) {
		// スタミナ切れ
		player_->ChangeState(new PlayerExhaustedState());
		return;
	}
	
	if (isMove) {
		player_->ChangeState(new PlayerWalkState());
		return;
	}

	if (player_->CanDash()) {
		// 走る
		player_->ChangeState(new PlayerDashState());
		return;
	}

	if (player_->HasAttackBuffer()) {
		// 攻撃
		player_->ChangeState(new PlayerAttackState(0));
		return;
	}
	ImGuiManager::GetInstance()->Text("StopState");
}

void PlayerStopState::Exit() {
	player_->UnBlockMove();
}