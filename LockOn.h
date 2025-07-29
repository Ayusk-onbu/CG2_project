#pragma once
#include "SpriteObject.h"
#include "CameraBase.h"
#include "Enemy.h"
#include "Player.h"

/// <summary>
/// ロックオンシステム
/// </summary>
class LockOn
{
public:
	void Initialize(SpriteObject& sprite,Texture&tex);
	void Update(Player*player,std::list<Enemy*>&enemies,CameraBase&camera);
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light);
private:
	SpriteObject sprite_;
	Texture* texture_ = nullptr;
	// ロックオン中の敵
	Enemy* targetEnemy_ = nullptr; // 現在ロックオン中の敵
};

