#pragma once
#include "ParticleData.h"
#include "ModelObject.h"
#include <json.hpp>
using json = nlohmann::json;

class ForceField
{
public:
	// 場の種類
	enum class Type {
		kGravity,
		kPoint,
		kVolume
	};
public:
	ForceField(Type type) :type_(type) {}
	virtual ~ForceField() = default;

	virtual void Initialize(Fngine* fngine) = 0;

	virtual void ApplyForce(ParticleData* info) = 0;

	virtual void DrawDebug() = 0;

	virtual void DrawImGui() = 0;

	virtual std::string GetName() = 0;

	virtual json SaveData() = 0;

	virtual void LoadData(const json& data) = 0;

	// 場の位置
	WorldTransform worldTransform_;
	Type type_;

	// 設定
	float strength_ = 1.0f;
	float radius_ = 50.0f; // 影響範囲
	bool isVisible_ = false;

	std::string name_;

	// DebugView
	std::unique_ptr<ModelObject>obj_;
};

class GravityForceField
	: public ForceField
{
public:
	GravityForceField(const Vector3& direction = { 0.0f,-9.8f * 5.0f,0.0f })
		: ForceField(Type::kGravity), direction_(direction) {}

	void Initialize(Fngine* fngine)override;

	void ApplyForce(ParticleData* info)override;

	void DrawDebug()override;

	void DrawImGui()override;

	std::string GetName()override { return "GravityForce" + name_; }

	json SaveData()override;

	void LoadData(const json& data)override;

	Vector3 direction_;
};

// 点源からの引力/斥力場（ブラックホール、爆発など）
class PointForceField : public ForceField
{
public:
	PointForceField(float strength = 100.0f)
		: ForceField(Type::kPoint) {
		strength_ = strength;
	}

	void Initialize(Fngine* fngine)override;

	void ApplyForce(ParticleData* info) override;

	void DrawDebug()override;

	void DrawImGui()override;

	std::string GetName()override { return "PointForce" + name_; }

	json SaveData()override;

	void LoadData(const json& data)override;
};
