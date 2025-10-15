#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"

// ================================
// Override Functions
// ================================

void TestScene::Initialize() {
	// Initialization code for the game scene
	mainCamera_ = std::make_unique<Camera>();
	mainCamera_->Initialize();
	useCamera_ = &(*mainCamera_);

	
}

void TestScene::Update() {
	useCamera_->Update();

	if (InputManager::GetKey().PressKey(DIK_0)) {
		hasRequestedNextScene_ = true;
	}

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {
}