#pragma once
#include "Structures.h"

class Enemy;

class EnemyState {
public:
	virtual void UpDate(Enemy* enemy,Vector3* pos,Vector3* rotation) = 0;
	virtual bool IsShot() = 0;
};

class EnemyStateApproach
	:public EnemyState
{
public:
	void UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation)override;
	bool IsShot() { return true; }
private:
	const float kMoveSpeed_ = 0.1f;
	bool initialize_ = false;
};

class EnemyStateLeave
	:public EnemyState
{
public:
	void UpDate(Enemy* enemy, Vector3* pos, Vector3* rotation)override;
	bool IsShot() { return false; }
private:
	const Vector3 kLeaveSpeed_ = { -0.05f,0.05f,0.1f };
};

