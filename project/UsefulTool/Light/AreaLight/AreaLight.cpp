#include "AreaLight.h"
#include "Fngine.h"
#include "ltc_matrix.cpp"

void AreaLight::Initialize(Fngine* fngine) {
	// 初期化処理
	// [ リソース作成 ]
	resource_ = CreateBufferResource(fngine->GetD3D12System().GetDevice().Get(), sizeof(AreaLightData));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	ltc1Resource_ = CreateLTCLevelTexture(fngine, const_cast<float*>(LTC1));
	ltc2Resource_ = CreateLTCLevelTexture(fngine, const_cast<float*>(LTC2));

	// LTCテクスチャ用のSRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 重要：LTCデータはFloat4
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // テクスチャとして扱う
	srvDesc.Texture2D.MipLevels = 1;

	ltc1CPUHandle_ = fngine->GetSRV().GetCPUDescriptorHandle();
	ltc1GPUHandle_ = fngine->GetSRV().GetGPUDescriptorHandle();

	ltc2CPUHandle_ = fngine->GetSRV().GetCPUDescriptorHandle();
	ltc2GPUHandle_ = fngine->GetSRV().GetGPUDescriptorHandle();

	// デスクリプタヒープの「次の空き」を取得して作成
	// ※LTC1用のSRVを作成
	ID3D12Device* device = fngine->GetD3D12System().GetDevice().Get();
	device->CreateShaderResourceView(ltc1Resource_.Get(), &srvDesc, ltc1CPUHandle_);

	// ※LTC2用のSRVを作成
	device->CreateShaderResourceView(ltc2Resource_.Get(), &srvDesc, ltc2CPUHandle_);

	data_->color = { 1.0f,1.0f,1.0f,1.0f };
	data_->intensity = 5.0f;
	data_->twoSided = 1;

	data_->points[0] = { -0.5f,  0.5f, 0.0f, 0.0f };
	data_->points[1] = { 0.5f,  0.5f, 0.0f, 0.0f };
	data_->points[2] = { 0.5f, -0.5f, 0.0f, 0.0f };
	data_->points[3] = { -0.5f, -0.5f, 0.0f, 0.0f };
}

void AreaLight::Update() {
#ifdef USE_IMGUI
	if (ImGui::TreeNode("Area Light Settings")) {
		// 色の編集
		ImGui::ColorEdit4("Color", &data_->color.x);

		// 強度の編集
		ImGui::DragFloat("Intensity", &data_->intensity, 0.1f, 0.0f, 100.0f);

		// 両面発光の切り替え
		bool twoSided = data_->twoSided != 0;
		if (ImGui::Checkbox("Two Sided", &twoSided)) {
			data_->twoSided = twoSided ? 1 : 0;
		}

		// 4つの頂点の編集
		if (ImGui::TreeNode("Vertices (Points)")) {
			for (int i = 0; i < 4; ++i) {
				std::string label = "Point[" + std::to_string(i) + "]";
				ImGui::DragFloat3(label.c_str(), &data_->points[i].x, 0.1f);
			}

			// おまけ：ライト全体を平行移動させる簡易ボタンなど
			static Vector3 offset = { 0,0,0 };
			ImGui::DragFloat3("Batch Offset", &offset.x, 0.1f);
			if (ImGui::Button("Apply Offset")) {
				for (int i = 0; i < 4; ++i) {
					data_->points[i].x += offset.x;
					data_->points[i].y += offset.y;
					data_->points[i].z += offset.z;
				}
				offset = { 0,0,0 }; // リセット
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
#endif
}

Microsoft::WRL::ComPtr<ID3D12Resource> AreaLight::CreateLTCLevelTexture(Fngine* fngine, float* data) {
	ID3D12Device* device = fngine->GetD3D12System().GetDevice().Get();

	// 1. テクスチャの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = 64;
	desc.Height = 64;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// 2. Texture.cpp と同じ特殊なヒープ設定 (Write-Back)
	// これにより WriteToSubresource が使用可能になります
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	// 3. データの転送 (WriteToSubresource を使用)
	// LTCデータは 64x64 の RGBA (float4) なので
	UINT rowPitch = 64 * 4 * sizeof(float);
	UINT slicePitch = rowPitch * 64;

	hr = resource->WriteToSubresource(
		0,
		nullptr,    // 全領域
		data,       // 元データ(LTC1 or LTC2)
		rowPitch,
		slicePitch
	);
	assert(SUCCEEDED(hr));
	return resource;
}