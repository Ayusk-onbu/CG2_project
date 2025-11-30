#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include <json.hpp>
using json = nlohmann::json;

class Particle;
class Fngine;

class ParticleEmitter
{
public:
	ParticleEmitter() = default;
public:
	void Initialize(Fngine* fngine);

	void Update(Particle* particleSystem);

	void Emit(Particle* particleSystem);

	void DrawDebug();

	void SetMinLifeTime(float time) { minLifeTime_ = time; }

	void SetMaxLifeTime(float time) { maxLifeTime_ = time; }

public:
	std::string name_;
	// エミッターの位置情報
	WorldTransform worldTransform_;

	uint32_t emitNum_ = 1;// 生成する数
	float spawnRadius_ = 1.0f;// エミッターの半径

	Vector3 minVelocity_ = { -1.0f,-1.0f,-1.0f };
	Vector3 maxVelocity_ = { 1.0f,1.0f,1.0f };

	float minLifeTime_ = 4.0f;
	float maxLifeTime_ = 5.0f;

	Vector4 startColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 endColor_ = { 1.0f,1.0f,1.0f,0.0f };

	Fngine* fngine_;
	std::unique_ptr<ModelObject>obj_;
};

