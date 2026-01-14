#pragma once
#include "Fngine.h"
#include "WinApp.h"
#include "D3D12ResourceLeakChecker.h"
#include "Chronos.h"
#include "SceneDirector.h"
#include "MathUtils.h"
#include "GlobalVariables.h"

class Game
{
public:

	// 初期化処理
	void Initialize();

	// ゲーム処理
	void Run();

	// 解放処理
	void Finalize();

	bool IsEnd() { return endRequest_; }
private:
	std::unique_ptr<Fngine> fngine_ = nullptr;
	std::unique_ptr<WinApp> winApp_ = nullptr;
	std::unique_ptr<SceneDirector> scene_ = nullptr;
	bool endRequest_ = false;
};

