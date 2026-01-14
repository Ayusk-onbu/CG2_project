#include "PlayerState.h"
#include "Player3D.h"

void PlayerWalkState::Initialize() {

}

void PlayerWalkState::Update() {
	bool isMove = false;
	isMove = player_->IsMoving();

	if (isMove && InputManager::IsJump()) {
		// 歩きながらジャンプで回避
		player_->ChangeState(new PlayerEvasionState());
		return;
	}

	//if (InputManager::IsJump()) {
	//	// ジャンプした
	//	player_->ChangeState(new PlayerJumpState());
	//	return;
	//}

	if (isMove == false) {
		// 動きが無かったら止まっているとみなす
		player_->ChangeState(new PlayerStopState());
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

	ImGuiManager::GetInstance()->Text("WalkState");
}

void PlayerWalkState::Exit() {

}