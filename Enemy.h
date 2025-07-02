#pragma once
#include "Transform.h"
#include "ModelObject.h"
#include "CameraBase.h"
#include <numbers>

class Player; // 前方宣言

class Enemy
{

	static inline const float kWalkSpeed = 0.01f;

	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 3.0f * std::numbers::pi_v<float>;
	static inline const float kWalkMotionTime = 1.0f;

public:
	void Initialize(ModelObject* model, CameraBase* camera, const Vector3& position);
	void UpDate();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	Transform& GetWorldTransform() { return worldTransform_; }
	void OnCollision(const Player* player);

private:

	// ワールド変換データ
	Transform worldTransform_;
	Transform uvTransform_;
	// モデル
	ModelObject* model_ = nullptr;
	// カメラ
	CameraBase* camera_ = nullptr;

	Vector3 velocity_ = {};
	float walkTimer_ = 0.0f;

};

