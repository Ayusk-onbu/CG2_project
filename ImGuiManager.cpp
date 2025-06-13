#include "ImGuiManager.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

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
