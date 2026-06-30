#pragma once
#include "Structures.h"
#include "ResourceFunc.h"

struct CameraForGPUData {
	Vector3 worldPosition;
};

struct OutlineForGPU {
	Matrix4x4 viewProjInverse;
};

struct DissolveConfigForGPU{
	float threshold = 0.5f;// しきいち
	float edgeMin = 0.5f;
	float edgeMax = 0.53f;
};

struct RandomConfigForGPU {
	float time = 0.0f;
};

class Fngine;

class CameraForGPU {
public:
	void Initialize(Fngine* fngine);
	void Update(const Vector3& cameraPos) { data_->worldPosition = cameraPos; }
	Microsoft::WRL::ComPtr<ID3D12Resource>&GetResource() { return resource_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	CameraForGPUData* data_;
};