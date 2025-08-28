#include "Nazo.h"
void Nazo::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	uvTransform_.Initialize();
	worldTransform_.set_.Translation({ 5.0f, 11.0f, -1.0f });
	worldTransform_.set_.Scale({ 2.0f, 2.0f, 10.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), Deg2Rad(15) });
}

void Nazo::Update() {
	// Update the world transform and UV transform if necessary
	// If you have any specific update logic for the ground, add it here
	Vector3 uvPos = uvTransform_.get_.Translation();
	uvPos.x += 0.1f;
	uvTransform_.set_.Translation(uvPos);
}

void Nazo::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}