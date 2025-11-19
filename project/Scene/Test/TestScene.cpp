#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

// ================================
// Override Functions
// ================================

TestScene::~TestScene() {
	delete particle_;
}

void TestScene::Initialize() {
	// Initialization code for the game scene
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->SetActiveCamera("DebugCamera");
	//player_.Initialize(p_fngine_);
	particle_ = new Particle(p_fngine_);
	particle_->Initialize(10);
}

void TestScene::Update() {

	if (InputManager::GetKey().PressKey(DIK_0)) {
		hasRequestedNextScene_ = true;
	}
	CameraSystem::GetInstance()->Update();
	particle_->Update();
	//player_.Update();
#ifdef _DEBUG
	ImGui::Begin("BlendMode");
	if (ImGui::Button("Alpha")) {
		PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::AlphaBlend);
	}
	if (ImGui::Button("Add")) {
		PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::Additive);
	}
	if (ImGui::Button("Sub")) {
		PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::Subtractive);
	}
	if (ImGui::Button("Mul")) {
		PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::Multiplicative);
	}
	if (ImGui::Button("Screen")) {
		PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::ScreenBlend);
	}
	ImGui::End();
#endif // _DEBUG

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {
	//player_.Draw();
	particle_->Draw();
}