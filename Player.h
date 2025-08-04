#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"
#include "Enemy.h"

class Player
	:public Collider
{
public:
	void Initialize(ModelObject* model, CameraBase* camera,Enemy&enemy);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

public:
	void OnCollision()override;
	const Vector3 GetWorldPosition()override;
	const Vector3 GetWorldPosition() const;
	const bool IsJump()const;
private:
	void Move(Vector3& pos);
	void Jump(Vector3& pos);

private:
	ModelObject* model_;
	CameraBase* camera_;
	Enemy* enemy_ = nullptr; // Pointer to the enemy for collision detection

	WorldTransform worldTransform_;
	WorldTransform uvTransform_;

	// Jump
	float jumpCount_ = 0;
	float jumpMaxCount_ = 0.275f; // Maximum number of jumps allowed
	uint32_t jumpState_ = 0; // 0: not jumping, 1: jumping, 2: falling
	bool isJump_ = false;
	uint32_t hp_ = 0;
	uint32_t maxHp_ = 100;
};

