#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"

class Enemy
{
public:
	void Initialize(D3D12System& d3d12, ModelObject& model, CameraBase* camera,const Vector3&pos);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	void Move(Vector3& pos);
	void Rotate(Vector3& rotation);
private:
	D3D12System* d3d12_;

	ModelObject model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;

	const float kMoveSpeed_ = 0.1f;
};

