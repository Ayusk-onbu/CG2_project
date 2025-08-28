#include "hallway.h"

void Hallway::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Scale({ 1.0f, 1.0f, 1.0f });
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 50.0f, 1.0f, 50.0f });
}

void Hallway::Update() {
	Vector3 position = worldTransform_.get_.Translation();
	Vector3 uvPos = uvTransform_.get_.Translation();
	uvPos.x += 0.1f;
	uvTransform_.set_.Translation(uvPos);

	worldTransform_.set_.Translation(position);
}

void Hallway::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_,uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void Hallway::OnCollision() {

}

const Vector3 Hallway::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}