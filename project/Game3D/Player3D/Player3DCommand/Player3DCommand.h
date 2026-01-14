#pragma once
#include "Structures.h"

class Player3D;

class Player3DCommand {
public:
	virtual ~Player3DCommand() = default;
	virtual void execute() = 0;
	void SetPlayer(Player3D* player) { player_ = player; }
protected:
	Player3D* player_ = nullptr;
};

class Player3DMoveCommand :
	public Player3DCommand {
public:
	Player3DMoveCommand() = default;
	void execute() override;
};

class Player3DJumpCommand :
	public Player3DCommand {
public:
	Player3DJumpCommand() = default;
	void execute() override;
};

class Player3DAttackCommand :
	public Player3DCommand {
public:
	Player3DAttackCommand() = default;
	void execute() override;
};

class Player3DDashCommand :
	public Player3DCommand {
public:
	Player3DDashCommand() = default;
	void execute() override;
};

class Player3DEvasionCommand :
	public Player3DCommand {
public:
	Player3DEvasionCommand() = default;
	void execute() override;
};