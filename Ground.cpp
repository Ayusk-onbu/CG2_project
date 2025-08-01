#include "Ground.h"

void Ground::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation({ 0.0f, -10.0f, 0.0f });
	worldTransform_.LocalToWorld();
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 4.0f, 4.0f, 1.0f });
}

void Ground::Update() {
	UvUpdate();
}

void Ground::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void Ground::UvUpdate() {
	Vector3 pos = uvTransform_.get_.Translation();
	Vector3 rot = uvTransform_.get_.Rotation();

	pos.y += 0.0001f;
	pos.x += 0.0001f;

	uvTransform_.set_.Translation(pos);
	uvTransform_.set_.Rotation(rot);
}