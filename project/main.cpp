#include "Game.h"
#include "WinApp.h"

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3D12ResourceLeakChecker leakCheck;
	
	std::unique_ptr<Game>game = std::make_unique<Game>();
	game->Initialize();
	std::unique_ptr<WinApp> winApp = std::make_unique<WinApp>();

	while (true) {
		if (winApp->ProcessMessage() || game->IsEnd()) {
			break;
		}
		game->Run();
	}

	game->Finalize();
	return 0;
}

//   決まり事
//   1. 長さはメートル(m)
//   2. 重さはキログラム(kg)
//   3. 時間は秒(s)