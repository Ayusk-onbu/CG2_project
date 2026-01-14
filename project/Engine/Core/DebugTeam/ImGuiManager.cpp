#include "ImGuiManager.h"
#include "Chronos.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

// =================================
// Core Functions
// =================================

void ImGuiManager::SetImGui(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap) {
#ifdef USE_IMGUI
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
#endif
}

void ImGuiManager::BeginFrame() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::EndFrame(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList) {
#ifdef USE_IMGUI
	// ImGuiについての情報を集める
	ImGui::Render();
	// 描画コマンドを実行する
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
#endif
}

void ImGuiManager::Shutdown()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

// =================================
// ImGui Functions
// =================================

void ImGuiManager::DrawAll() {
#ifdef USE_IMGUI
	ImGui::Begin("Test");

	if (ImGui::Button("IsDebugImGuiView")) {
		isDebugImGuiView_ = !isDebugImGuiView_;
	}

	for (auto& func : imGuiFunctions_) {
		func();
	}

	if (ImGui::Button("FPS 30")) {
		Chronos::GetInstance()->SetTargetFPS(30.0f);
	}
	if (ImGui::Button("FPS 60")) {
		Chronos::GetInstance()->SetTargetFPS(60.0f);
	}
	if (ImGui::Button("FPS 80")) {
		Chronos::GetInstance()->SetTargetFPS(80.0f);
	}
	if (ImGui::Button("FPS 120")) {
		Chronos::GetInstance()->SetTargetFPS(120.0f);
	}
	if (ImGui::Button("IsFixed Change")) {
		Chronos::GetInstance()->ChangeIsFixed();
	}
	if (Chronos::GetInstance()->GetIsFixed()) {
		ImGui::Text("IsFixed :True");
	}
	else {
		ImGui::Text("IsFixed :false");
	}

	ImGui::Text("FPS %f", static_cast<float>(Chronos::GetInstance()->GetFPS()));
	ImGui::Text("FPS(ImGui) %f",ImGui::GetIO().Framerate);

	ImGui::End();

	imGuiFunctions_.clear();
	if (isDebugImGuiView_) {
		ImGui::ShowDemoWindow();
		ImGui::ShowStyleEditor();
	}
#endif // USEIMGUI
}

// ---- [ Text ] ----

void ImGuiManager::Text(const char* text) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=]() {
		ImGui::Text(text);
	});
#endif
}

// ---- [ Vector4 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector4& value, float min, float max) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat4(label, &value.x, min, max);
	});
#endif
}

void ImGuiManager::DrawDrag(const char* label, Vector4& value) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat4(label, &value.x);
	});
#endif
}

// ---- [ Vector3 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector3& value, float min, float max) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat3(label, &value.x, min, max);
	});
#endif
}

void ImGuiManager::DrawDrag(const char* label, Vector3& value) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat3(label, &value.x);
	});
#endif
}

// ---- [ Vector2 ] ----

void ImGuiManager::DrawSlider(const char* label, Vector2& value, float min, float max) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat2(label, &value.x, min, max);
		});
#endif
}

void ImGuiManager::DrawDrag(const char* label, Vector2& value) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat2(label, &value.x);
	});
#endif
}

// ---- [ float ] ----

void ImGuiManager::DrawSlider(const char* label, float& value, float min, float max) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::SliderFloat(label, &value, min, max);
	});
#endif
}

void ImGuiManager::DrawDrag(const char* label, float& value) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::DragFloat(label, &value);
	});
#endif
}

// ---- [ Matrix4x4 ] ----

void ImGuiManager::DrawSlider(const char* label, Matrix4x4& value, float min, float max) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::Text(label); // ラベルを表示
		for (int i = 0; i < 4; ++i) {
			ImGui::SliderFloat4(
				(std::string(label) + " [" + std::to_string(i) + "]").c_str(),
				&value.m[i][0], min, max
			);
		}
	});
#endif
}

void ImGuiManager::DrawDrag(const char* label, Matrix4x4& value) {
#ifdef USE_IMGUI
	imGuiFunctions_.push_back([=, &value]() {
		ImGui::Text(label); // ラベルを表示
		for (int i = 0; i < 4; ++i) {
			ImGui::DragFloat4(
				(std::string(label) + " [" + std::to_string(i) + "]").c_str(),
				&value.m[i][0]
			);
		}
	});
#endif
}