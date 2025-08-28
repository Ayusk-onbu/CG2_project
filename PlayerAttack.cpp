#include "PlayerStates.h"
#include "Player.h"
#include "InputManager.h" 
#include "ImGuiManager.h"

void PlayerStateAttack::Initialize()
{
	// Initialization code for attack state
	attackTime_ = 0.0f; // Reset attack time
}

void PlayerStateAttack::Update()
{
	if (p_Player_ == nullptr) return;
	attackTime_ += 1.0f / 60.0f; // Increment attack time based on frame rate



	
	if (attackTime_ >= attackMaxTime_) {
		p_Player_->ChangeState(new PlayerStateStop());
	}
	// Player remains in attack state, no movement logic needed
}