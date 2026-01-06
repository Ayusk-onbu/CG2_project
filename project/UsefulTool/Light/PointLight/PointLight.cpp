#include "PointLight.h"
#include "Fngine.h"

void PointLight::Initialize(Fngine* fngine) {
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(PointLightData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	data_->color = { 1.0f,0.2f,0.4f,1.0f };//白色
	data_->position = { 0.0f,4.0f,0.0f };//原点
	data_->intensity = 1.0f;//輝度1.0f
	data_->radius = 45.0f;//影響範囲
	data_->decay = 2.0f;//減衰率
}

void PointLight::Update() {
	// ImGui
	ImGuiManager::GetInstance()->DrawSlider("PointLight : Color", data_->color, 0.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawDrag("PointLight : Position", data_->position);
	ImGuiManager::GetInstance()->DrawSlider("PointLight : Intensity", data_->intensity, 0.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawDrag("PointLight : radius", data_->radius);
	ImGuiManager::GetInstance()->DrawDrag("PointLight : decay", data_->decay);
}