#pragma once
#include "GuideCurve.h"
#include "Structured.h"

class Hear {
public:
	void Initialize(Fngine* engine);
	void Update(float deltaTime);
	void Render();
private:
	Microsoft::WRL::ComPtr<ID3D12StateObject> CreateHairRaytracingPSO(
		ID3D12Device5* device, // DXR対応のDevice5が必要
		ID3D12RootSignature* globalRootSignature,
		const void* shaderByteCode,
		SIZE_T shaderByteCodeSize);
private:
	GuideCurve::GuideHear guide_;

	std::unique_ptr<Structured<Strands::StrandVertex>> hairAABBBuffer_;

	Fngine* engine_ = nullptr;
};