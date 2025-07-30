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
	~LockOn() {
		for (SpriteObject* sprite : sprites_) {
			delete sprite;
		}
		sprites_.clear();
	}
	void Initialize(D3D12System* d3d12,SpriteObject& sprite,Texture&tex);
	void Update(Player*player,std::list<Enemy*>&enemies,CameraBase&camera);
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light);
private:
	D3D12System* d3d12_ = nullptr; // D3D12システム
	SpriteObject sprite_;
	Texture* texture_ = nullptr;
	// ロックオン中の敵
	Enemy* targetEnemy_ = nullptr; // 現在ロックオン中の敵
	std::list<Enemy*> targetEnemies_; // ロックオン可能な敵のリスト
	std::list<SpriteObject*> sprites_; // ロックオン中の敵のスプライトリスト
};

