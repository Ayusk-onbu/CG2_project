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

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF(
		"resources/Font/NotoSansJP-Regular.ttf",
		18.0f,
		nullptr,
		io.Fonts->GetGlyphRangesJapanese()
	);
#endif
}

void ImGuiManager::BeginFrame() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
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
	ImGui::Text("DeltaTime %f", static_cast<float>(Chronos::GetInstance()->GetDeltaTime()));
	ImGui::Text("ゲーム開始からのリアル経過時間 %f", static_cast<float>(Chronos::GetInstance()->GetTotalTime()));
	ImGui::Text("ゲーム開始からのゲーム内経過時間 %f", static_cast<float>(Chronos::GetInstance()->GetGameTime()));

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

void ImGuiManager::DrawGizmo(Matrix4x4& view, Matrix4x4& projection, Vector3& translation, Vector3& rotation, Vector3& scale, ImGuizmo::OPERATION operation, ImGuizmo::MODE mode) {
#ifdef USE_IMGUI
	Matrix4x4 viewCopy = view;
	Matrix4x4 projCopy = projection;

	// 後で一括描画（遅延実行）した際、呼び出し元のSRT変数を直接書き換えるためにポインタを保持
	Vector3* pTrans = &translation;
	Vector3* pRot = &rotation;
	Vector3* pScale = &scale;

	imGuiFunctions_.push_back([=]() {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::Begin("##GizmoFullscreenWindow", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		// --- 単位変換の準備（エンジンがラジアン管理の場合） ---
		const float kRadToDeg = 180.0f / 3.14159265f;
		const float kDegToRad = 3.14159265f / 180.0f;

		// ImGuizmo用に一時的にデグリー（度数）に変換した回転角を作る
		Vector3 rotDeg = { pRot->x * kRadToDeg, pRot->y * kRadToDeg, pRot->z * kRadToDeg };

		// 1. 現在のSRTから、ギズモ用の一時的なワールド行列を合成（Recompose）する
		Matrix4x4 objectMatrix = {};
		ImGuizmo::RecomposeMatrixFromComponents(
			reinterpret_cast<float*>(pTrans),
			reinterpret_cast<float*>(&rotDeg),
			reinterpret_cast<float*>(pScale),
			reinterpret_cast<float*>(&objectMatrix)
		);

		// 2. ギズモの描画とマウス操作を実行（戻り値が true の時はマウスで動かされた瞬間）
		if (ImGuizmo::Manipulate(
			reinterpret_cast<const float*>(&viewCopy),
			reinterpret_cast<const float*>(&projCopy),
			operation,
			mode,
			reinterpret_cast<float*>(&objectMatrix)
		)) {
			// 3. マウス操作で変化した行列を、SRT成分に分解（Decompose）して元の変数に書き戻す！
			ImGuizmo::DecomposeMatrixToComponents(
				reinterpret_cast<float*>(&objectMatrix),
				reinterpret_cast<float*>(pTrans),
				reinterpret_cast<float*>(&rotDeg),
				reinterpret_cast<float*>(pScale)
			);

			// 分解されたデグリー角をラジアンに戻して、オブジェクトの実際の変数に適用
			pRot->x = rotDeg.x * kDegToRad;
			pRot->y = rotDeg.y * kDegToRad;
			pRot->z = rotDeg.z * kDegToRad;
		}

		ImGui::End();
		});
#endif
}

//void ImGuiManager::DrawGizmo(Matrix4x4& view, Matrix4x4& projection, Matrix4x4& objectMatrix, ImGuizmo::OPERATION operation, ImGuizmo::MODE mode) {
//#ifdef USE_IMGUI
//	// 【対策1】カメラ行列はその時点の「値」を確実にコピーして保持する
//	Matrix4x4 viewCopy = view;
//	Matrix4x4 projCopy = projection;
//
//	// オブジェクトの行列のポインタを保持
//	Matrix4x4* pObjectMat = &objectMatrix;
//
//	imGuiFunctions_.push_back([=]() {
//		ImGuiIO& io = ImGui::GetIO();
//
//		// 【対策2】ギズモ用に画面全体を覆う「透明なウィンドウ」を強制的に作る
//		ImGui::SetNextWindowPos(ImVec2(0, 0));
//		ImGui::SetNextWindowSize(io.DisplaySize);
//		ImGui::Begin("##GizmoFullscreenWindow", nullptr,
//			ImGuiWindowFlags_NoTitleBar |
//			ImGuiWindowFlags_NoResize |
//			ImGuiWindowFlags_NoScrollbar |
//			ImGuiWindowFlags_NoInputs |
//			ImGuiWindowFlags_NoSavedSettings |
//			ImGuiWindowFlags_NoBackground); // 背景透過
//
//		// 【対策3】この透明ウィンドウの描画リストを使うと明示する
//		ImGuizmo::SetDrawlist();
//		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//
//		// ギズモの描画と操作を実行
//		ImGuizmo::Manipulate(
//			reinterpret_cast<const float*>(&viewCopy),
//			reinterpret_cast<const float*>(&projCopy),
//			operation,
//			mode,
//			reinterpret_cast<float*>(pObjectMat)
//		);
//
//		ImGui::End(); // 透明ウィンドウを閉じる
//		});
//#endif
//}