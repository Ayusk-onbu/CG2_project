#include "ImGuiManager.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// =================================
// Core Functions
// =================================

void ImGuiManager::SetImGui(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device.Get(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList) {
	// ImGuiについての情報を集める
	ImGui::Render();
	// 描画コマンドを実行する
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}

void ImGuiManager::Shutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// =================================
// ImGui Functions
// =================================

void ImGuiManager::DrawAll() {
#ifdef _DEBUG
	ImGui::Begin("Test");

	if (ImGui::Button("IsDebugImGuiView")) {
		isDebugImGuiView_ = !isDebugImGuiView_;
	}

	for (auto& func : imGuiFunctions_) {
		func();
	}



	ImGui::End();

	imGuiFunctions_.clear();
	if (isDebugImGuiView_) {
		ImGui::ShowDemoWindow();
		ImGui::ShowStyleEditor();
	}
#endif // _DEBUG
}

// ---- [ Text ] ----

void ImGuiManager::Text(const char* text) {
	imGuiFunctions_.push_back([=]() {
		ImGui::Text(text);
	});
}

// ---- [ Vector4 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector4& value, float min, float max) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat4(label, &value.x, min, max);
	});
}

void ImGuiManager::DrawDrag(const char* label, Vector4& value) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat4(label, &value.x);
	});
}

// ---- [ Vector3 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector3& value, float min, float max) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat3(label, &value.x, min, max);
	});
}

void ImGuiManager::DrawDrag(const char* label, Vector3& value) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat3(label, &value.x);
	});

}

// ---- [ Vector2 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector2& value, float min, float max) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat2(label, &value.x, min, max);
		});
}

void ImGuiManager::DrawDrag(const char* label, Vector2& value) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat2(label, &value.x);
	});
}

// ---- [ float ] ----

void ImGuiManager::DrawSlider(const char* label, float& value, float min, float max) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat(label, &value, min, max);
	});
}

void ImGuiManager::DrawDrag(const char* label, float& value) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat(label, &value);
	});
}

// ---- [ Matrix4x4 ] ----

void ImGuiManager::DrawSlider(const char* label, Matrix4x4& value, float min, float max) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::Text(label); // ラベルを表示
		for (int i = 0; i < 4; ++i) {
			ImGui::SliderFloat4(
				(std::string(label) + " [" + std::to_string(i) + "]").c_str(),
				&value.m[i][0], min, max
			);
		}
	});
}

void ImGuiManager::DrawDrag(const char* label, Matrix4x4& value) {
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::Text(label); // ラベルを表示
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string(label) + " [" + std::to_string(i) + "]").c_str(),
				&value.m[i][0]
			);
		}
	});
}