#pragma once
#include "ResourceFunc.h"
#include "Structures.h"

struct SpotLightData {
	Vector4 color;// ライトの色
	Vector3 position;// ライトの位置
	float intensity;// 輝度
	Vector3 direction;// ライトの向き
	float distance;// 影響範囲
	float decay;// 減衰率
	float cosAngle;// 余弦
	float cosFalloffStartAngle;//開始の余弦
	float padding[1];
};

const uint32_t kMaxSpotLights = 10;

struct MultiSpotLightData {
	uint32_t activeLightCount;// 使用しているライトの数
	float padding[3];
	SpotLightData lights[kMaxSpotLights];
};

class Fngine;

class SpotLight
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	void SetPosition(int num, const Vector3& pos) { data_->lights[num].position = pos; }
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	MultiSpotLightData* data_ = nullptr;
	float angles_[kMaxSpotLights];
	float falloffs_[kMaxSpotLights];
};

