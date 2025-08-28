#include "Pot.h"
#include "ImGuiManager.h"
#include "InputManager.h"

void Pot::Initialize(ModelObject* model, CameraBase* camera,Fngine& fngine) {
	model_ = std::make_unique<ModelObject>();
	model_->Initialize(fngine.GetD3D12System(), model->GetModelData());
	camera_ = camera;

	worldTransform_.Initialize();
	uvTransform_.Initialize();
}

void Pot::Update() {
	//ImGui::Begin("Pot");
	//ImGui::Text("Pot");
	//ImGui::DragFloat3("Position", &worldTransform_.set_.Translation().x, 0.1f);
	//ImGui::DragFloat3("Rotation", &worldTransform_.set_.Rotation().x, 0.1f);
	//ImGui::DragFloat3("Scale", &worldTransform_.set_.Scale().x, 0.1f, 0.1f);
	//ImGui::End();
	Vector3 uvPos = uvTransform_.get_.Translation();
	uvPos.x += 0.01f;
	uvTransform_.set_.Translation(uvPos);
}

void Pot::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void Pot::OnCollision() {
	Vector3 position = worldTransform_.get_.Translation();
	position.y += 0.5f; // Raise the pot slightly on collision
	worldTransform_.set_.Translation(position); // Reset position on collision
}

const Vector3 Pot::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}