#include "PlayerState.h"
#include "Player3D.h"

void PlayerWalkState::Initialize() {

}

void PlayerWalkState::Update() {
	bool isMove = false;
	auto& move = player_->Move();
	if (InputManager::GetKey().PressKey(DIK_W))
	{
		move.z = 1.0f;
		isMove = true;
	}
	if (InputManager::GetKey().PressKey(DIK_S))
	{
		move.z = -1.0f;
		isMove = true;
	}
	if (InputManager::GetKey().PressKey(DIK_A))
	{
		move.x = -1.0f;
		isMove = true;
	}
	if (InputManager::GetKey().PressKey(DIK_D))
	{
		move.x = 1.0f;
		isMove = true;
	}

	if (isMove && InputManager::IsJump()) {
		// 歩きながらジャンプで回避
		player_->ChangeState(new PlayerEvasionState());
		return;
	}

	if (InputManager::IsJump()) {
		// ジャンプした
		player_->ChangeState(new PlayerJumpState());
		return;
	}

	if (isMove == false) {
		// 動きが無かったら止まっているとみなす
		player_->ChangeState(new PlayerStopState());
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

	ImGuiManager::GetInstance()->Text("WalkState");
}