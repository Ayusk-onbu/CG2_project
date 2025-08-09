#include "TitleScene.h"
#include "ImGuiManager.h"

void TitleScene::Initialize() {
	// Initialization code for the game scene
}

void TitleScene::Update() {
	ImGui::Begin("Title");
	ImGui::Text("Press Space");
	ImGui::End();
}

void TitleScene::Draw() {

}