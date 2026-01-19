#include "SpotLight.h"
#include "Fngine.h"

void SpotLight::Initialize(Fngine* fngine) {
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(MultiSpotLightData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	
	data_->activeLightCount = 10;

	for (uint32_t i = 0;i < kMaxSpotLights; ++i) {
		angles_[i] = 60.0f;
		falloffs_[i] = 0.9f;
		
		data_->lights[i].color = { 1.0f,1.0f,1.0f,1.0f };//白色
		data_->lights[i].position = { 0.0f,16.0f,0.0f };//原点
		data_->lights[i].intensity = 4.0f;//輝度1.0f
		data_->lights[i].distance = 45.0f;//影響範囲
		data_->lights[i].decay = 2.0f;//減衰率
		data_->lights[i].direction = { -0.0f,-1.0f,0.0f };
		data_->lights[i].cosAngle = std::cosf(Deg2Rad(angles_[i]));
		data_->lights[i].cosFalloffStartAngle = std::cosf(Deg2Rad(angles_[i] * falloffs_[i]));
	}
}

void SpotLight::Update() {
#ifdef USE_IMGUI
	ImGui::Begin("SpotLight Manager");

	for (uint32_t i = 0; i < data_->activeLightCount; ++i) {
		std::string label = "SpotLight [" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {
			// 既存の計算ロジックを配列に対して適用
			data_->lights[i].cosAngle = std::cosf(Deg2Rad(angles_[i]));
			data_->lights[i].cosFalloffStartAngle = std::cosf(Deg2Rad(angles_[i] * falloffs_[i]));

			ImGui::SliderFloat4("Color", &data_->lights[i].color.x, 0.0f, 1.0f);
			ImGui::DragFloat3("Position", &data_->lights[i].position.x);
			ImGui::SliderFloat("Intensity", &data_->lights[i].intensity, 0.0f, 10.0f);
			ImGui::SliderFloat3("Direction", &data_->lights[i].direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat("Angle", &angles_[i], 0.0f, 180.0f);
			ImGui::SliderFloat("Falloff", &falloffs_[i], 0.0f, 1.0f);

			ImGui::TreePop();
		}
	}
	ImGui::End();
#endif//USE_IMGUI
}


