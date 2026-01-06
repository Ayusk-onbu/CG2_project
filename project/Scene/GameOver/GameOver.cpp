#include "GameOver.h"
#include "CameraSystem.h"
#include "SceneDirector.h"

void GameOverScene::Initialize() {
	sprite_ = std::make_unique<SpriteObject>(p_fngine_);
	sprite_->Initialize(TextureManager::GetInstance()->LoadTexture("resources/GameOver.png"));
	sprite_->worldTransform_.set_.Translation({ 640.0f,360.0f,0.0f });
}

void GameOverScene::Update() {
	if (InputManager::IsJump()) {
		isTrans_ = true;
	}

	if (isTrans_) {
		p_sceneDirector_->RequestChangeScene(new TestScene);
	}
}

void GameOverScene::Draw() {
	sprite_->Draw();
}