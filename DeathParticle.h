#pragma once
#include <stdint.h>
#include <array>
#include "ModelObject.h"
#include "Transform.h"
#include "Structures.h"
#include "Camera.h"
#include "DebugCamera.h"

class DeathParticle {

	static inline const uint32_t kNumParticles = 8;
	// 消滅時間
	static inline const float kDuraction = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.01f;
	// 角度
	static inline const float kAngle = 2 * 3.141592f / kNumParticles;

public:
	void Initialize(ModelObject* model, Vector3& position);
	void UpDate();
	void Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

private:

	std::array<Transform, kNumParticles> worldTransform_;

	bool isFinished_ = false;
	float counter_ = 0.0f;

	Vector4 color_;

	ModelObject* model_;
	Transform uvTransform_;
};

