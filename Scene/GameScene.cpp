#include "GameScene.h"
#include "ModelObject.h"
#include "ImGuiManager.h"

void GameScene::Initialize() {
	// Initialization code for the game scene
}

void GameScene::Update(){
	ImGui::Begin("Game");
	ImGui::Text("Press Spcae");
	ImGui::End();
}

void GameScene::Draw() {

}