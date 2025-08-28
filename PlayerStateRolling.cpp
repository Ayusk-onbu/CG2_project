#include "PlayerStates.h"
#include "Player.h"
#include "InputManager.h" 
#include "ImGuiManager.h"

void PlayerStateRolling::Initialize()
{
	// Initialization code for rolling state
	rollingTime_ = 0.0f; // Reset roll time
	if (p_Player_ == nullptr) return;
	
	// Set rolling speed and direction based on current input
}

void PlayerStateRolling::Update()
{
	if (p_Player_ == nullptr) return;
	p_Player_->SetIsRolling(true); // Set rolling flag
	rollingTime_ += 1.0f / 60.0f; // Increment roll time based on frame rate

	Vector3 pos = p_Player_->GetWorldTransform().get_.Translation();
	float direction = p_Player_->GetDirection(); // Get current direction

	// Move the player in the current direction at rolling speed
	pos.x += direction * p_Player_->GetStats().GetAgility() * 0.2f; // Rolling speed is higher than walking

	p_Player_->GetWorldTransform().set_.Translation(pos);

	/*ImGui::Begin("Rolling");
	ImGui::Text("Rolling Time: %f", rollingTime_);
	ImGui::End();*/

	if (rollingTime_ >= rollingMaxTime_) {
		p_Player_->SetIsRolling(false); // Reset rolling flag
		p_Player_->ChangeState(new PlayerStateStop());
	}
	// Player remains in rolling state, no additional input handling needed
}