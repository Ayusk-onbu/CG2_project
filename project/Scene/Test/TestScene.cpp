#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"

// ================================
// Override Functions
// ================================

TestScene::~TestScene() {
	delete particle_;
	delete sprite_;
}

void TestScene::Initialize() {
	// Initialization code for the game scene
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->SetActiveCamera("DebugCamera");
	player_.Initialize(p_fngine_);
	particle_ = new Particle(p_fngine_);
	particle_->Initialize(7500);

	sprite_ = new SpriteObject(p_fngine_);
	sprite_->SetFlip(false,true);
	sprite_->Initialize(TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png"));
	sprite_->worldTransform_.set_.Translation({640.0f,360.0f,0.0f});

	CameraSystem::GetInstance()->GetActiveCamera()->SetRadius(100.0f);
	PSOManager::GetInstance()->GetPSO("Structured").SetBlendState(BLENDMODE::Additive);
}

void TestScene::Update() {

	//if (InputManager::GetKey().PressKey(DIK_0)) {
	//	hasRequestedNextScene_ = true;
	//}
	CameraSystem::GetInstance()->Update();
	particle_->Update();
	player_.Update();

	if (InputManager::IsAttack()) {
		sprite_->SetTextureSize({0.0f,0.0f}, {64.0f,64.0f});
	}

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
	particle_->DrawDebug();
	sprite_->Draw(SPRITE_VIEW_TYPE::Object);
	particle_->Draw();
}