#include "PlayerInputHandler.h"
#include "InputManager.h"

PlayerInputHandler::PlayerInputHandler()
	: moveCommand_(std::make_unique<Player3DMoveCommand>()) {

}

void PlayerInputHandler::HandleInput() {
	moveCommand_->execute();
	if (InputManager::IsJump()) {
		auto jumpCmd = std::make_unique<Player3DJumpCommand>();
		jumpCmd->SetPlayer(player_);
		jumpCmd->execute();
	}
	if (InputManager::IsAttack()) {
		auto attackCmd = std::make_unique<Player3DAttackCommand>();
		attackCmd->SetPlayer(player_);
		attackCmd->execute();
	}
	if (InputManager::IsDash()) {
		auto dashCmd = std::make_unique<Player3DDashCommand>();
		dashCmd->SetPlayer(player_);
		dashCmd->execute();
	}
	if (InputManager::TrigerEvasion()) {
		auto evasionCmd = std::make_unique<Player3DEvasionCommand>();
		evasionCmd->SetPlayer(player_);
		evasionCmd->execute();
	}
}