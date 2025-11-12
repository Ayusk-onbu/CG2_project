#include "ClearScene.h"
#include "CameraSystem.h"
#include "SceneDirector.h"

void ClearScene::Initialize() {
	sprite_.Initialize(p_fngine_->GetD3D12System(), 1280.0f, 720.0f);
	world_.Initialize();
	world_.set_.Scale({ 1.00f,1.00f,1.00f });
	world_.set_.Translation({ 0.0f,0.0f,0.0f });
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/GameClear.png");
}

void ClearScene::Update() {
	if (InputManager::IsJump()) {
		isTrans_ = true;
	}

	if (isTrans_) {
		p_sceneDirector_->RequestChangeScene(new GameScene);
	}
}

void ClearScene::Draw() {
	world_.LocalToWorld();
	sprite_.SetWVPData(
		CameraSystem::GetInstance()->GetActiveCamera()->DrawUI(world_.mat_),
		world_.mat_,
		Matrix4x4::Make::Identity()
	);
	sprite_.Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), TextureManager::GetInstance()->GetTexture(textureHandle_));
}