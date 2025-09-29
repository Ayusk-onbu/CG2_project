#include "GameScene.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
	
}

void GameScene::Initialize() {
	// Initialization code for the game scene
	mainCamera_ = std::make_unique<Camera>();
	mainCamera_->Initialize();
	fpsCamera_ = std::make_unique<Camera>();
	fpsCamera_->Initialize();

	useCamera_ = &(*mainCamera_);
	//SetCamera(*mainCamera_);
}

void GameScene::Update(){
	useCamera_->UpDate();
}

void GameScene::Draw() {
	
}