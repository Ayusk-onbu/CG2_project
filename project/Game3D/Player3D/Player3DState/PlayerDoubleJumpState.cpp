#include "PlayerState.h"
#include "Player3D.h"

void PlayerDoubleJumpState::Initialize() {
	// さらにジャンプ
	player_->SetVerticalVelocity(initial_verticalVelocity);
	// 二段ジャンプの権利は消費された
	player_->SetCanDoubleJump(false);
}

void PlayerDoubleJumpState::Update() {

	if (player_->HasAttackBuffer()) {
		// 攻撃
		player_->ChangeState(new PlayerAttackState(0));
		return;
	}

	if (player_->GetVerticalVelocity() <= 0.0f) {
		// 下がり始めたら空中にいる状態にする
		player_->ChangeState(new PlayerFallState());
		return;
	}

	// 一応
	if (player_->IsOnGround() == true) {
		// 地面についていたら一旦止まる
		player_->ChangeState(new PlayerStopState());
		return;
	}

	// 空中攻撃とかがあればここから遷移処理

	ImGuiManager::GetInstance()->Text("DoubleJumpState");
}

void PlayerDoubleJumpState::Exit() {

}