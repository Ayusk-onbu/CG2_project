#include "Wall.h"
void Wall::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	uvTransform_.Initialize();
	worldTransform_.set_.Translation({ 0.0f, 8.5f, 5.0f });
	worldTransform_.set_.Scale({ 2.5f, 2.0f, 1.0f });
	worldTransform_.set_.Rotation({ 0.0f,0.0f,0.0f });
	uvTransform_.set_.Scale({ 15.0f, 15.0f, 1.0f });
	model_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
}

void Wall::Update() {
	// Update the world transform and UV transform if necessary
	// If you have any specific update logic for the ground, add it here
}

void Wall::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}