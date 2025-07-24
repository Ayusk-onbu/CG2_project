#include "SkyDome.h"

void SkyDome::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.LocalToWorld();
	uvTransform_.Initialize();
}

void SkyDome::Update() {
	UvUpdate();
}

void SkyDome::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void SkyDome::UvUpdate() {
	Vector3 pos = uvTransform_.get_.Translation();
	Vector3 rot = uvTransform_.get_.Rotation();

	pos.y += 0.0001f;
	pos.x += 0.0001f;

	uvTransform_.set_.Translation(pos);
	uvTransform_.set_.Rotation(rot);
}