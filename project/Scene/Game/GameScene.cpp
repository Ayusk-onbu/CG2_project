#include "GameScene.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

GameScene::GameScene()
	: player_(std::make_unique<Player3D>())
{

}

GameScene::~GameScene() {
	
	Log::ViewFile("Path GameScene ~");
}

void GameScene::Initialize() {
	player_->Initialize(p_fngine_);

}

void GameScene::Update(){
	player_->Update();

}

void GameScene::Draw() {

	player_->Draw();
}