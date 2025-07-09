#pragma once
#include <array>
#include "Transform.h"
#include "ModelObject.h"
#include "CameraBase.h"

class DeathParticle
{

	static inline const uint32_t kNumParticles = 8;
	// 消滅時間
	static inline const float kDuraction = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 角度
	static inline const float kAngle = 2 * 3.141592f / kNumParticles;

public:
	void Initialize(const Vector3& position);
	void UpDate();
	bool IsFinished() const { return isFinished_; }
	void SetPosition(const Vector3& position);
	Matrix4x4 GetPosition(int i);
	Vector4& GetColor(){ return color_; }

private:

	std::array<Transform, kNumParticles> worldTransform_;
	Transform uvTransform_;

	bool isFinished_ = false;
	float counter_ = 0.0f;

	Vector4 color_;
};

