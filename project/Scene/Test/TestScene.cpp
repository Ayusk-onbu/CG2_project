#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

// ================================
// Override Functions
// ================================

TestScene::~TestScene() {
	
}

void TestScene::Initialize() {
	// 初期化処理

	player_ = std::make_unique<Player3D>();
	player_->Initialize(p_fngine_);
}

void TestScene::Update() {
	player_->Update();

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {
	player_->Draw();
}