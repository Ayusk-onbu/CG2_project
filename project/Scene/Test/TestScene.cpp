#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"

// ================================
// Override Functions
// ================================

void TestScene::Initialize() {
	// Initialization code for the game scene
	
	
}

void TestScene::Update() {

	if (InputManager::GetKey().PressKey(DIK_0)) {
		hasRequestedNextScene_ = true;
	}

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {
}