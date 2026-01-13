#include "Fngine.h"
#include "D3D12ResourceLeakChecker.h"
#include "Chronos.h"
#include "SceneDirector.h"
#include "MathUtils.h"
#include "GlobalVariables.h"


//   決まり事
//   1. 長さはメートル(m)
//   2. 重さはキログラム(kg)
//   3. 時間は秒(s)

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3D12ResourceLeakChecker leakCheck;
	std::unique_ptr<Fngine> fngine = std::make_unique<Fngine>();
	fngine->Initialize();

	std::unique_ptr<SceneDirector> scene = std::make_unique<SceneDirector>();

	GlobalVariables::GetInstance()->LoadFiles();

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
			InputManager::Update();
			GlobalVariables::GetInstance()->Update();

			fngine->BeginOSRFrame();
			scene->Run();
			fngine->EndOSRFrame();
			fngine->BeginFrame();

			ImGuiManager::GetInstance()->DrawAll();
			fngine->EndFrame();
		}
		
	}
	
	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
