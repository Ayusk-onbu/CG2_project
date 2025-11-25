#include "CameraForGPU.h"
#include "Fngine.h"

void CameraForGPU::Initialize(Fngine* fngine) {
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(CameraForGPUData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	data_->worldPosition = { 0.0f,0.0f,0.0f };
}