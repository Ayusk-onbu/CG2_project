#include "DirectionLight.h"

void DirectionLight::Initialize(D3D12System d3d12) {
	resource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(DirectionalLight));
	GetResourceAddress();
	//デフォルト値
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,0.0f,1.0f };
	directionalLightData_->intensity = 1.0f;
	directionalLightData_->shadowType = 2; // デフォルトのシャドウタイプ
}

void DirectionLight::SetType(const int& type) {
	directionalLightData_->shadowType = type;
}

void DirectionLight::GetResourceAddress() {
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
}
