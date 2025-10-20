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
#include <vector>
#include <functional>

class ImGuiManager
{
public:
	static ImGuiManager* GetInstance() {
		static ImGuiManager instance;
		return &instance;
	}
public:
	void SetImGui(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> srvDescriptorHeap);

	void BeginFrame();

	void EndFrame(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList);

	void Shutdown();

	void DrawAll();
public:
	//---- [ Usuful ]
	void Text(const char* text);

	//---- [ Vector4 ] ----
	void DrawSlider(const char* label, Vector4& value, float min, float max);
	void DrawDrag(const char* label, Vector4& value );

	//---- [ Vector3 ] ----
	void DrawSlider(const char* label, Vector3& value, float min, float max);
	void DrawDrag(const char* label, Vector3& value);

	//---- [ Vector2 ] ----
	void DrawSlider(const char* label, Vector2& value, float min, float max);
	void DrawDrag(const char* label, Vector2& value);

	//---- [ float ] ----
	void DrawSlider(const char* label, float& value, float min, float max);
	void DrawDrag(const char* label, float& value);

	//---- [ Matrix4x4 ] ----
	void DrawSlider(const char* label, Matrix4x4& value, float min, float max);
	void DrawDrag(const char* label, Matrix4x4& value);
	
private:
	std::vector<std::function<void()>> imGuiFunctions_{};

	bool isDebugImGuiView_ = false;
};

