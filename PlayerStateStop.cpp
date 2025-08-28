#include "PlayerStates.h"
#include "Player.h"
#include "InputManager.h" 
#include "ImGuiManager.h"

void PlayerStateStop::Initialize()
{
	// Initialization code for stop state
}

void PlayerStateStop::Update()
{
	if (p_Player_ == nullptr) return;

	

	// Check for input to change state
	if (InputManager::GetKey().PressKey(DIK_W) ||
		InputManager::GetKey().PressKey(DIK_A) ||
		InputManager::GetKey().PressKey(DIK_S) ||
		InputManager::GetKey().PressKey(DIK_D)) {
		p_Player_->ChangeState(new PlayerStateWalk());
		return;
	}

	if (InputManager::GetKey().PressKey(DIK_SPACE)){
		p_Player_->ChangeState(new PlayerStateAttack());
		return;
	}

	if (InputManager::GetKey().PressKey(DIK_LSHIFT)) {
		p_Player_->ChangeState(new PlayerStateRolling());
		return;
	}

	// Player remains stationary, no movement logic needed
}