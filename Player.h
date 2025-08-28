#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"
#include "Enemy.h"

#include "PlayerStats.h"
#include "PlayerStates.h"

class Player
	:public Collider
{
public:
	Player() = default;
	~Player() {
		if (state_) {
			delete state_;
		}
	}

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
public:
	void Initialize(ModelObject* model, CameraBase* camera);
	void Initialize();
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

public:
	void OnCollision()override;
	void ChangeState(PlayerState* state);
public:
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	const Vector3 GetWorldPosition()override;
	const Vector3 GetWorldPosition() const;
	PlayerStats& GetStats() { return stats_; }
	float GetDirection() const { return direction_; }
	bool IsAlive() const { return isAlive_; }
public:
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	void SetDirection(const float& direction) { direction_ = direction; }
	bool IsCollisionActive() const { return isCollisionActive_; }
	void SetCollisionActive(bool active) { isCollisionActive_ = active; }
	void SetColor(const Vector4& color) { model_->SetColor(color); }
	void SetIsAlive(bool alive) { isAlive_ = alive; }
	void SetIsRolling(bool rolling) { isRolling_ = rolling; }
private:

private:
	ModelObject* model_;
	CameraBase* camera_;
	Enemy* enemy_ = nullptr; // Pointer to the enemy for collision detection

	WorldTransform worldTransform_;
	WorldTransform uvTransform_;

	PlayerState* state_ = nullptr;
	PlayerStats stats_;
	bool isAlive_ = true;
	bool isRolling_ = false; // Is the player rolling

	float direction_ = 0.0f;
	bool isCollisionActive_ = true; // Flag to check if collision is active
};

