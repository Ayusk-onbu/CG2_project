#pragma once
#include "ResourceFunc.h"
#include "Structures.h"

struct PointLightData {
	Vector4 color;// ライトの色
	Vector3 position;// ライトの位置
	float intensity;// 輝度
	float radius;// ライトの影響範囲
	float decay;// 減衰率
	float padding[2];
};

const uint32_t kMaxPointLights = 100;

struct  MultiPointLightData {
	uint32_t numLights;
	float padding[3];
	PointLightData lights[kMaxPointLights];
};

class Fngine;

class PointLight
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	void SetHeartPosition();
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	MultiPointLightData* data_ = nullptr;
};

