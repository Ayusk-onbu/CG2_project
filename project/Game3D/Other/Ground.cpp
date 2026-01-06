#include "Ground.h"

void Ground::Initialize(Fngine* fngine) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = "ground";
	obj_->textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/Legends_Ground.png");
	obj_->Initialize(fngine);
}

void Ground::Update() {

}

void Ground::Draw() {
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}