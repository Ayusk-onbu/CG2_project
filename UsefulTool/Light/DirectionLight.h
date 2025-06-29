#pragma once
#include <wrl.h>
#include "ResourceFunc.h"
#include "D3D12System.h"

struct DirectionalLight {
	Vector4 color;// ライトの色
	Vector3 direction;// ライトの向き
	float intensity;// 輝度
};

class DirectionLight
{
public:
	void Initialize(D3D12System d3d12);
private:
	void GetResourceAddress();
#pragma region GetSeries
public:
	Microsoft::WRL::ComPtr <ID3D12Resource>GetResource() { return resource_; }
#pragma endregion
public:
	DirectionalLight* directionalLightData_ = nullptr;
private:
	Microsoft::WRL::ComPtr <ID3D12Resource> resource_;
};

