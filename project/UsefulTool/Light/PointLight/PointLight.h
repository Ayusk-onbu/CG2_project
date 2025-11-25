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

class Fngine;

class PointLight
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	PointLightData* data_ = nullptr;
};

