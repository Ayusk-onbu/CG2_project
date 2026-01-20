#include "Ground.h"

void Ground::Initialize(Fngine* fngine) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = "ground";
	obj_->textureName_ = "Legends_Ground";
	obj_->Initialize(fngine);
}

void Ground::Update() {

}

void Ground::Draw() {
	ImGuiManager::GetInstance()->DrawDrag("Ground:Shininess", obj_->materialData_->shininess);
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}