#pragma once
#include <memory>
#include <map>
#include "Player3DCommand.h"

class PlayerInputHandler{
public:
	PlayerInputHandler();
public:
	void SetPlayer(Player3D* player) { player_ = player; moveCommand_->SetPlayer(player); }
	void HandleInput();
private:
	std::unique_ptr<Player3DMoveCommand> moveCommand_;
	Player3D* player_ = nullptr;
};

