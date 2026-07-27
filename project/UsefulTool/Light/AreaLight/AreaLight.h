#pragma once
#include "ResourceFunc.h"
#include "Structures.h"
#include "ShaderResourceView.h"

struct AreaLightData {
	Vector4 color;
	Vector4 points[4];
	float intensity;
	int32_t twoSided;
	float padding[2];
};

class Fngine;

class AreaLight {
public:
	void Initialize(Fngine* fngine);
	void Update();
	Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource() { return resource_; }
	ID3D12Resource* GetLTC1() { return ltc1Resource_.Get(); }
	ID3D12Resource* GetLTC2() { return ltc2Resource_.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE& GetLTC1GPUHandle() { return ltc1Allocation_.gpu; }
	D3D12_GPU_DESCRIPTOR_HANDLE& GetLTC2GPUHandle() { return ltc2Allocation_.gpu; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateLTCLevelTexture(Fngine* fngine, float* data);
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> ltc1Resource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> ltc2Resource_;

	SRVAllocation ltc1Allocation_;
	SRVAllocation ltc2Allocation_;
	AreaLightData* data_ = nullptr;
};

