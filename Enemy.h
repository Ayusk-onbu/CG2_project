#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"

class Enemy
{
public:
	enum class Phase {
		Approach,
		Leave,
	};

	void Initialize(D3D12System& d3d12, ModelObject& model, CameraBase* camera,const Vector3&pos);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	void ApproachUpdate(Vector3& pos, Vector3& rotation);
	void ApproachMove(Vector3& pos);
	void ApproachRotate(Vector3& rotation);

	void LeaveUpdate(Vector3& pos, Vector3& rotation);
	void LeaveMove(Vector3& pos);
	void LeaveRotate(Vector3& rotation);
private:
	static void (Enemy::*pFuncTable[])(Vector3& pos, Vector3& rotation);
private:
	D3D12System* d3d12_;

	ModelObject model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;

	Phase phase_ = Phase::Approach;
	const float kMoveSpeed_ = 0.1f;
	const Vector3 kLeaveSpeed_ = {-0.05f,0.05f,0.1f};
};

