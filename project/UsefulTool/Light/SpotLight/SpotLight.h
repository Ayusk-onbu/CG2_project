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

class Fngine;

class SpotLight
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	SpotLightData* data_ = nullptr;
	float angle_ = 60.0f;
	float falloff_ = 0.9f;
};

