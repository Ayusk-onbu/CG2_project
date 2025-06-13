#pragma once
#include <Windows.h>
#include <wrl.h>
#pragma region ImGui系
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma endregion

#include <type_traits>
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

class ImGuiManager
{
public:
	static void SetImGui(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap);

	static void BeginFrame();

	static void EndFrame(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList);

	static void Shutdown();

	static void CreateImGui(const char* label, Vector4& value, float min, float max) {
		ImGui::SliderFloat4(label, &value.x, min, max);
	}

	static void CreateImGui(const char* label, Vector3& value, float min, float max) {
		ImGui::SliderFloat3(label, &value.x, min, max);
	}

	static void CreateImGui(const char* label, Vector2& value, float min, float max) {
		ImGui::SliderFloat2(label, &value.x, min, max);
	}

	static void CreateImGui(const char* label, float& value, float min, float max) {
		ImGui::SliderFloat(label, &value, min, max);
	}

	
};

