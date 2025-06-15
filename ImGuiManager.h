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
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include <string>

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

	static void CreateImGui(const char* label, Matrix4x4& value, float min, float max) {
		ImGui::Text(label); // ラベルを表示

		for (int i = 0; i < 4; ++i) {
			ImGui::SliderFloat4(
				(std::string(label) + " [" + std::to_string(i) + "]").c_str(),
				&value.m[i][0], min, max
			);
		}
	}
	
};

