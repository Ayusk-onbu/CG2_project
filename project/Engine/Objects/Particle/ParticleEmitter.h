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

	json SaveData();
	void LoadData(const json& data);

	void DrawDebug();

	void SetMinLifeTime(float time) { minLifeTime_ = time; }

	void SetMaxLifeTime(float time) { maxLifeTime_ = time; }

public:
	// 外部情報のポインタ
	Fngine* fngine_;
public:
	// *** エミッターに関する情報 ***
	// [ Emitterの名前 ]
	std::string name_;

	// [ エミッターの位置情報 ]
	WorldTransform worldTransform_;

	// [ 生成する数 ]
	uint32_t emitNum_ = 1;
	// [ エミッターの半径 ] 
	float spawnRadius_ = 1.0f;

	// [ 移動速度 ]
	Vector3 minVelocity_ = { -1.0f,-1.0f,-1.0f };
	Vector3 maxVelocity_ = { 1.0f,1.0f,1.0f };

	// [ 寿命 ]
	float minLifeTime_ = 4.0f;
	float maxLifeTime_ = 5.0f;

	// [ 色 ]
	Vector4 startColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 endColor_ = { 1.0f,1.0f,1.0f,0.0f };
	
	// [ デバッグ関係 ]
	std::unique_ptr<ModelObject>obj_;
};

