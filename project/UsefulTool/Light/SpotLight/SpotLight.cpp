#include "SpotLight.h"
#include "Fngine.h"

void SpotLight::Initialize(Fngine* fngine) {
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(SpotLightData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	data_->color = { 1.0f,1.0f,1.0f,1.0f };//白色
	data_->position = { 2.0f,1.25f,0.0f };//原点
	data_->intensity = 4.0f;//輝度1.0f
	data_->distance = 7.0f;//影響範囲
	data_->decay = 2.0f;//減衰率
	data_->direction = { -1.0f,-1.0f,0.0f };
	data_->cosAngle = std::cosf(Deg2Rad(angle_));
	data_->cosFalloffStartAngle = std::cosf(Deg2Rad(angle_ * falloff_));
}

void SpotLight::Update() {
	data_->cosAngle = std::cosf(Deg2Rad(angle_));
	data_->cosFalloffStartAngle = std::cosf(Deg2Rad(angle_ * falloff_));
	// ImGui
	ImGuiManager::GetInstance()->DrawSlider("SpotLight : Color", data_->color, 0.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawDrag("SpotLight : Position", data_->position);
	ImGuiManager::GetInstance()->DrawSlider("SpotLight : Intensity", data_->intensity, 0.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawDrag("SpotLight : distance", data_->distance);
	ImGuiManager::GetInstance()->DrawDrag("SpotLight : decay", data_->decay);
	ImGuiManager::GetInstance()->DrawSlider("SpotLight : direction", data_->direction,-1.0f,1.0f);
	ImGuiManager::GetInstance()->DrawSlider("SpotLight : angle", angle_, -360.0f, 360.0f);
	ImGuiManager::GetInstance()->DrawDrag("SpotLight : falloff", data_->cosFalloffStartAngle);
	ImGuiManager::GetInstance()->DrawSlider("SpotLight : *falloff", falloff_,0.01f,0.9f);
}


