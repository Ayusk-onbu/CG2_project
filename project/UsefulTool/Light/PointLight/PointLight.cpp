#include "PointLight.h"
#include "Fngine.h"
#include <iostream>

void PointLight::Initialize(Fngine* fngine) {
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(MultiPointLightData));
	//書き込むためのアドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	// Lightの数を登録
	data_->numLights = 100;
	data_->numLights = data_->numLights < kMaxPointLights ? data_->numLights : kMaxPointLights;
	for (uint32_t i = 0; i < kMaxPointLights; ++i) {
		// ライトの色を設定
		data_->lights[i].color = {1.0f,0.2f,0.4f,1.0f};
		// 位置の設定
		data_->lights[i].position = { 0.0f + i * 0.5f,1.5f,0.0f };
		// 輝度の設定
		data_->lights[i].intensity = 1.0f;
		// 影響範囲の設定
		data_->lights[i].radius = 3.0f;
		// 減衰率の設定
		data_->lights[i].decay = 2.0f;
	}

	SetHeartPosition();
}

void PointLight::Update() {
	// ImGui
#ifdef USE_IMGUI
	if (ImGui::TreeNode("PointLights")) {
		for (uint32_t i = 0; i < data_->numLights; ++i) {
			// 1. 各項目用のラベル文字列を生成（変数に保持することで寿命を確保）
			std::string treeLabel = "PointLight[" + std::to_string(i) + "]";
			// Treeなどを使い、ライトごとに開閉できるようにすると見やすくなります
			if (ImGui::TreeNode(treeLabel.c_str())) {

				std::string colorLabel = "Color##" + std::to_string(i);
				std::string posLabel = "Position##" + std::to_string(i);
				std::string intLabel = "Intensity##" + std::to_string(i);
				std::string radLabel = "Radius##" + std::to_string(i);
				std::string decLabel = "Decay##" + std::to_string(i);

				// 2. 生成したラベルを使用して描画
				ImGui::SliderFloat4(colorLabel.c_str(), &data_->lights[i].color.x, 0.0f, 1.0f);
				ImGui::DragFloat3(posLabel.c_str(), &data_->lights[i].position.x);
				ImGui::SliderFloat(intLabel.c_str(), &data_->lights[i].intensity, 0.0f, 1.0f);
				ImGui::DragFloat(radLabel.c_str(), &data_->lights[i].radius);
				ImGui::DragFloat(decLabel.c_str(), &data_->lights[i].decay);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
#endif//USE_IMGUI
}

void PointLight::SetHeartPosition() {
	data_->numLights = 100; // 10個使用
	int index = 0;
	float spacing = 1.0f; // ライトの間隔

	for (int y = 0; y < 10; ++y) {
		for (int x = 0; x < 10; ++x) {
			// グリッド座標を -1.0 ~ 1.0 の範囲に正規化
			float px = (x - 4.5f) / 4.5f;
			float py = (y - 4.5f) / 4.5f;

			// ハートの数式 (簡易版)
			// (x^2 + y^2 - 1)^3 - x^2 * y^3 <= 0
			float formula = pow(px * px + py * py - 1.0f, 3.0f) - (px * px * py * py * py);

			data_->lights[index].position = { x * spacing - 10.0f, 0.0f, y * spacing - 20.0f };

			if (formula <= 0.0f) {
				// ハートの内側：赤く光らせる
				data_->lights[index].color = { 1.0f, 0.6f, 0.6f, 1.0f };
				data_->lights[index].intensity = 1.0f;
			}
			else {
				// ハートの外側：暗くして見えなくする
				data_->lights[index].color = { 0.6f, 0.2f, 0.2f, 1.0f };
				data_->lights[index].intensity = 0.2f;
			}

			data_->lights[index].radius = 1.0f;
			data_->lights[index].decay = 2.0f;
			index++;
		}
	}
}