#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"

class Player
{
public:
	void Initialize(std::unique_ptr<ModelObject>model, CameraBase* camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	void Move(Vector3& pos,float& rotation);
private:
	std::unique_ptr<ModelObject>model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;

	const float kMoveSpeed_ = 0.1f;
	const Vector2 kMoveLimitX_ = { 15.0f,8.5f };
};

