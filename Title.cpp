#include "Title.h"

void Title::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Scale({ 5.0f, 5.0f, 1.0f });
	worldTransform_.set_.Translation({ 0.0f, 9.0f, -4.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), 0.0f});
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 10.0f, 10.0f, 1.0f });
}

void Title::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.set_.Scale({ 5.0f, 5.0f, 1.0f });
	worldTransform_.set_.Translation({ 0.0f, 9.0f, -4.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), 0.0f });
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 10.0f, 10.0f, 1.0f });
}

void Title::Update() {
	Vector3 position = worldTransform_.get_.Translation();
	Vector3 uvPos = uvTransform_.get_.Translation();

	uvPos.x += 0.01f;

	worldTransform_.set_.Translation(position);
	uvTransform_.set_.Translation(uvPos);
}

void Title::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void Title::Move(float t) {
	Vector3 pos = worldTransform_.get_.Translation();
	pos.y = (1.0f - t) * 9.0f + t * -25.0f;
	worldTransform_.set_.Translation(pos);
}

void Title::OnCollision() {

}

const Vector3 Title::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}