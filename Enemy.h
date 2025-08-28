#pragma once
#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"
class Player;
class Enemy
	:public Collider
{
public:
	void Initialize(ModelObject* model, CameraBase* camera);
	void Update(const Player&player);
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

public:
	void OnCollision()override;
	const Vector3 GetWorldPosition()override;
	const Vector3 GetDirection() const { return direction_; }
	bool IsAlive() const { return isAlive_; }
private:
	void Move(Vector3& pos, const Player& player);
private:
	ModelObject* model_;
	CameraBase* camera_;

	WorldTransform worldTransform_;
	WorldTransform uvTransform_;

	bool isAlive_ = true; // Is the enemy alive
	float speed_ = 0.0f; // Speed of the enemy
	Vector3 direction_;
	float acceleration_ = 0.001f;
};

