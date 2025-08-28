#include "FailedClear.h"

void FailedClear::Initialize(ModelObject* modelClear, ModelObject* modelFailed,CameraBase* camera) {
	modelFailed_ = modelFailed;
	modelClear_ = modelClear;

	modelFailed_->SetColor({ 0.1f, 0.1f, 1.0f, 1.0f });
	modelClear_->SetColor({ 1.0f, 1.0f, 0.1f, 1.0f });

	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Scale({ 5.0f, 5.0f, 1.0f });
	worldTransform_.set_.Translation({ 0.0f, 6.0f, -4.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), 0.0f });
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 10.0f, 10.0f, 1.0f });
}

void FailedClear::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.set_.Scale({ 5.0f, 5.0f, 1.0f });
	worldTransform_.set_.Translation({ 0.0f, 6.0f, -4.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), 0.0f });
	uvTransform_.Initialize();
	uvTransform_.set_.Scale({ 10.0f, 10.0f, 1.0f });
}

void FailedClear::Update() {
	Vector3 position = worldTransform_.get_.Translation();
	Vector3 uvPos = uvTransform_.get_.Translation();

	uvPos.x += 0.01f;

	worldTransform_.set_.Translation(position);
	uvTransform_.set_.Translation(uvPos);
}

void FailedClear::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	if(isClear_){
		model_ = modelClear_;
	}
	else {
		model_ = modelFailed_;
	}
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}

void FailedClear::Move(float t) {
	Vector3 pos = worldTransform_.get_.Translation();
	pos.y = (1.0f - t) * 9.0f + t * -25.0f;
	worldTransform_.set_.Translation(pos);
}