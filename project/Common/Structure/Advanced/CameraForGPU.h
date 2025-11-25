#pragma once
#include "Structures.h"
#include "ResourceFunc.h"

struct CameraForGPUData {
	Vector3 worldPosition;
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