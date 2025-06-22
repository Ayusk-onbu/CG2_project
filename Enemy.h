#pragma once
#include "ModelObject.h"
#include "Transform.h"
#include "Camera.h"
#include "DebugCamera.h"
#include <numbers>

class Player;

class Enemy
{

	static inline const float kWalkSpeed = 0.01f;

	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 3.0f * std::numbers::pi_v<float>;
	static inline const float kWalkMotionTime = 1.0f;

public:
	void Initialize(ModelObject* model, Vector3& pos);
	void UpDate();
	void Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	Transform& GetWorldTransform() { return worldTransform_; }
	void OnCollision(const Player* player);

private:

	// ワールド変換データ
	Transform worldTransform_;
	// モデル
	ModelObject* model_ = nullptr;
	Transform uvTransform_;
	
	Vector3 velocity_ = {};
	float walkTimer_ = 0.0f;

};

