#include "PlayerStates.h"
#include "Player.h"
#include "InputManager.h" 
#include "ImGuiManager.h"

void PlayerStateWalk::Initialize()
{
	// Initialization code for walking state
}

void PlayerStateWalk::Update()
{
	if (p_Player_ == nullptr) return;
	isMove_ = 0; // Reset movement flags
	float direction = p_Player_->GetDirection(); // Get current direction

	Vector3 pos = p_Player_->GetWorldTransform().get_.Translation();
	if (InputManager::GetKey().PressKey(DIK_W)) { isMove_ += 1 << 0; }
	if (InputManager::GetKey().PressKey(DIK_A)) { isMove_ += 1 << 1; }
	if (InputManager::GetKey().PressKey(DIK_S)) { isMove_ += 1 << 2; }
	if (InputManager::GetKey().PressKey(DIK_D)) { isMove_ += 1 << 3; }

	if (isMove_ & 0b0001) {
		// 上を押した
	}
	if (isMove_ & 0b0010) {
		// 左を押した
		pos.x -= p_Player_->GetStats().GetAgility() * 0.1f; // Move left
		p_Player_->SetDirection(-1.0f); // Adjust direction
	}
	if (isMove_ & 0b0100) {
		// 下を押した
	}
	if (isMove_ & 0b1000) {
		// 右を押した
		pos.x += p_Player_->GetStats().GetAgility() * 0.1f; // Move right
		p_Player_->SetDirection(1.0f); // Adjust direction
	}
	if (isMove_ ==  0b1010) {
		p_Player_->SetDirection(direction); // Adjust direction
	}

	// Check for collision with the enemy
	/*if (p_Player_->GetEnemy() != nullptr) {
		p_Player_->OnCollision();
	}*/

	p_Player_->GetWorldTransform().set_.Translation(pos);
	
	
	if (InputManager::GetKey().PressKey(DIK_SPACE)) {
		p_Player_->ChangeState(new PlayerStateAttack());
		return;
	}
	if (InputManager::GetKey().PressKey(DIK_LSHIFT)) {
		p_Player_->ChangeState(new PlayerStateRolling());
		return;
	}
	if (isMove_ == 0) {
		// If no movement keys are pressed, change to stop state
		p_Player_->ChangeState(new PlayerStateStop());
		return;
	}
}