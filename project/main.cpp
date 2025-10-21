#include "Fngine.h"
#include "Chronos.h"
#include "RandomUtils.h"
#include "SceneDirector.h"
#include "MathUtils.h"
#include "WorldTransform.h"

struct D3D12ResourceLeakChecker {
	~D3D12ResourceLeakChecker() {
		// リソースリーク✅
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		//リソースリークチェック==================-↓↓↓
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//debug->Release();
		}
		//リソースリークチェック==================-↑↑↑
	}
};

//   決まり事
//   1. 長さはメートル(m)
//   2. 重さはキログラム(kg)
//   3. 時間は秒(s)

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3D12ResourceLeakChecker leakCheck;
	std::unique_ptr<Fngine> fngine = std::make_unique<Fngine>();
	fngine->Initialize();

	TimeRandom::Initialize();

	std::unique_ptr<SceneDirector> scene = std::make_unique<SceneDirector>();
	scene->SetUpFngine(*fngine);
	scene->Initialize(*new TestScene());
	
	
	MSG msg{};

	//   基礎的な物の処理

	//ウィンドウのｘボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		
		//Widnowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ImGuiManager::GetInstance()->BeginFrame();
			Chronos::Update();
			InputManager::Update();
#pragma region OffScreenRendering
			/*ResourceBarrier barrierO = {};
			barrierO.SetBarrier(command.GetList().GetList().Get(), osr.GetResource().Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
			osr.Begin(command);*/
			//ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::descriptorHeap_.GetHeap().Get() };
			//command.GetList().GetList()->SetDescriptorHeaps(1, descriptorHeaps);
			///////////////////////////////////////////////////////////////////////////
			////描画0200
			//command.GetList().GetList()->RSSetViewports(1, &viewport);
			//command.GetList().GetList()->RSSetScissorRects(1, &scissorRect);

			

			//model.SetWVPData(debugCamera.DrawMirrorCamera(worldMatrix, transformSprite.translate, {0.0f,0.0f,-1.0f}), worldMatrix, uvTransformMatrix);

			//model.Draw(command, pso, light, tex);

			//barrierO.SetTransition(command.GetList().GetList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
#pragma endregion
			fngine->BeginFrame();
			scene->Run();
			ImGuiManager::GetInstance()->DrawAll();
			fngine->EndFrame();
		}
		
	}
	
	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
