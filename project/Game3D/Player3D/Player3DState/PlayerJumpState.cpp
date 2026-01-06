#include "PlayerState.h"
#include "Player3D.h"

void PlayerJumpState::Initialize() {
	player_->SetVerticalVelocity(initial_verticalVelocity);
	player_->SetIsOnGround(false);
}

void PlayerJumpState::Update() {

	if (player_->GetVerticalVelocity() <= 0.0f) {
		// 下がり始めたら空中にいる状態にする
		player_->ChangeState(new PlayerFallState());
		return;
	}

	//if (player_->CanDoubleJump() && InputManager::IsJump()) {
	//	// 二段ジャンプへ
	//	player_->ChangeState(new PlayerDoubleJumpState());
	//	return;
	//}

	// 一応
	if (player_->IsOnGround() == true) {
		// 地面についていたら一旦止まる
		player_->ChangeState(new PlayerStopState());
		return;
	}

	if (player_->HasAttackBuffer()) {
		// 攻撃
		player_->ChangeState(new PlayerAttackState(0));
		return;
	}

	ImGuiManager::GetInstance()->Text("JumpState");
}

void PlayerJumpState::Exit() {

}