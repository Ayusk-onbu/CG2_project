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

		// --- ライブ演出用の初期設定 ---
		basePosition_[i] = { (i - 5.0f) * 10.0f, 20.0f, 0.0f }; // 横一列に配置
		speedOffset_[i] = 1.0f + (static_cast<float>(rand()) / RAND_MAX) * 2.0f; // 回転速度のバラつき
		radiusOffset_[i] = 5.0f + (static_cast<float>(rand()) / RAND_MAX) * 10.0f; // 振れ幅のバラつき
		
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

	// デルタタイム（固定値でも可）分だけタイマーを進める
	timer_ += 0.03f;

	for (uint32_t i = 0; i < data_->activeLightCount; ++i) {
		// --- 1. 激しい円運動 ---
		float phase = timer_ * speedOffset_[i];
		data_->lights[i].position.x = basePosition_[i].x + std::sin(phase) * radiusOffset_[i];
		data_->lights[i].position.z = basePosition_[i].z + std::cos(phase) * radiusOffset_[i];

		// --- 2. 常に中心（ステージ）を向くような演出 ---
		// 座標(0,0,0)を狙うように方向を計算
		Vector3 target = { 0.0f, 0.0f, 0.0f };
		data_->lights[i].direction.x = target.x - data_->lights[i].position.x;
		data_->lights[i].direction.y = target.y - data_->lights[i].position.y;
		data_->lights[i].direction.z = target.z - data_->lights[i].position.z;

		// --- 3. 虹色に変化するカラー演出 ---
		// インデックスごとに位相をずらしてカラフルに
		data_->lights[i].color.x = std::sin(phase * 0.5f) * 0.5f + 0.5f;
		data_->lights[i].color.y = std::sin(phase * 0.5f + 2.0f) * 0.5f + 0.5f;
		data_->lights[i].color.z = std::sin(phase * 0.5f + 4.0f) * 0.5f + 0.5f;

		// --- 4. ビートに合わせて点滅（ストロボ効果） ---
		//if (std::sin(timer_ * 10.0f) > 0.8f) {
		//	data_->lights[i].intensity = 15.0f; // 瞬間的に明るく
		//}
		//else {
		//	data_->lights[i].intensity = 5.0f;
		//}

		// 角度計算の更新
		data_->lights[i].cosAngle = std::cosf(Deg2Rad(angles_[i]));
		data_->lights[i].cosFalloffStartAngle = std::cosf(Deg2Rad(angles_[i] * falloffs_[i]));
	}

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


