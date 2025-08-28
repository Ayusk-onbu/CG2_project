#pragma once
#include <cstdint>

class Player;

class PlayerState
{
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void GetPlayer(Player& player) { p_Player_ = &player; }
protected:
	Player* p_Player_;
};

class PlayerStateStop
	: public PlayerState
{
public:
	void Initialize() override;
	void Update() override;
private:

};

class PlayerStateWalk
	: public PlayerState
{
public:
	void Initialize()override;
	void Update() override;
private:
	uint32_t isMove_ = 0;
};

class PlayerStateAttack
	:public PlayerState
{
public:
	void Initialize()override;
	void Update() override;
private:
	float attackTime_ = 0.0f; // Time since the last attack
	float attackMaxTime_ = 2.0f; // Maximum time for the attack animation
};

class PlayerStateRolling
	:public PlayerState
{
public:
	void Initialize()override;
	void Update() override;
private:

	float rollingTime_ = 0.0f; // Time since the last roll
	float rollingMaxTime_ = 0.75f; // Maximum time for the roll animation
};

