#include "GameScene.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

GameScene::~GameScene() {
	
	Log::ViewFile("Path GameScene ~");
}

void GameScene::Initialize() {
	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera",CameraType::Debug);
	CameraSystem::GetInstance()->SetActiveCamera("NormalCamera");

	player_->Initialize(p_fngine_);
}

void GameScene::Update(){
	
	CameraSystem::GetInstance()->Update();
	player_->Update();

	ImGui::Begin("BlendMode");
	if (ImGui::Button("Alpha")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::AlphaBlend);
	}
	if (ImGui::Button("Add")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Additive);
	}
	if (ImGui::Button("Sub")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Subtractive);
	}
	if (ImGui::Button("Mul")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Multiplicative);
	}
	if (ImGui::Button("Screen")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::ScreenBlend);
	}
	if (ImGui::Button("ChangeDebug")) {
		CameraSystem::GetInstance()->SetActiveCamera("DebugCamera");
	}
	if (ImGui::Button("ChangeNormal")) {
		CameraSystem::GetInstance()->SetActiveCamera("NormalCamera");
	}
	if (ImGui::Button("AddDebug")) {
		CameraSystem::GetInstance()->GetActiveCamera()->AddControllers(CameraType::Debug);
	}
	ImGui::End();
}

void GameScene::Draw() {
	player_->Draw();
}