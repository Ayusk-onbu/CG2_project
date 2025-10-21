#include "GameScene.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

GameScene::~GameScene() {
	
	Log::ViewFile("Path GameScene ~");
}

void GameScene::Initialize() {
	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera",CameraType::Debug);
	CameraSystem::GetInstance()->SetActiveCamera("DebugCamera");

}

void GameScene::Update(){
	
	CameraSystem::GetInstance()->Update();

}

void GameScene::Draw() {

}