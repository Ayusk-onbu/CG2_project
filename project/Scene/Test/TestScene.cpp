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

	/*player_ = std::make_unique<Player3D>();
	player_->Initialize(p_fngine_);*/
	title_ = std::make_unique<SpriteObject>(p_fngine_);
	title_->Initialize("titleBack");
	title_->worldTransform_.set_.Translation({640.0f,360.0f,0.0f});

	fade_ = std::make_unique<SpriteObject>(p_fngine_);
	fade_->Initialize("GridLine");
	fade_->worldTransform_.set_.Translation({ 640.0f,360.0f,0.0f });
	fade_->worldTransform_.set_.Scale({ 1280.0f / 16.0f,1280.0f / 16.0f ,0.0f });
	fade_->SetColor({ 0.0f,0.0f,0.0f,0.0f });

	toGameTimer_ = 0.0f;

	particle_ = std::make_unique<Particle>(p_fngine_);
	particle_->Initialize(1000);
}

void TestScene::Update() {
	//player_->Update();

	particle_->Update();

	if (InputManager::IsJump()) {
		hasRequestedNextScene_ = true;
	}

	if (hasRequestedNextScene_) {
		toGameTimer_ += 1.0f / 60.0f;
		float alpha = 0.0f;
		float endTime = 2.0f;
		alpha = Easing_Float(0.0f, 1.0f, toGameTimer_, endTime, EASINGTYPE::InSine);
		fade_->SetColor({ 0.0f,0.0f,0.0f,alpha });
		if (toGameTimer_ > endTime) {
			p_sceneDirector_->RequestChangeScene(new GameScene());
		}
	}
}

void TestScene::Draw() {
	
	particle_->Draw();
	
	//player_->Draw();
	//title_->Draw();
	//fade_->Draw();
}