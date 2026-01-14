#include "Game.h"

void Game::Initialize() {
	fngine_ = std::make_unique<Fngine>();
	winApp_ = std::make_unique<WinApp>();
	scene_ = std::make_unique<SceneDirector>();
	fngine_->Initialize();
	GlobalVariables::GetInstance()->LoadFiles();
	scene_->SetUpFngine(*fngine_);
	scene_->Initialize(*new TestScene());
}

void Game::Run() {
	if (winApp_->ProcessMessage()) {
		endRequest_ = true;
	}

	ImGuiManager::GetInstance()->BeginFrame();
	InputManager::Update();
	GlobalVariables::GetInstance()->Update();

	fngine_->BeginOSRFrame();
	scene_->Update();
	scene_->Draw();
	fngine_->EndOSRFrame();

	fngine_->BeginFrame();
	scene_->ImGui();
	ImGuiManager::GetInstance()->DrawAll();
	fngine_->EndFrame();
}

void Game::Finalize() {
	fngine_.reset();
	//COMの初期化を解除
	CoUninitialize();
}