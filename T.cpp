#include "T.h"
void T::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	uvTransform_.Initialize();
	worldTransform_.set_.Translation({ -15.0f, 20.0f, 0.0f });
	worldTransform_.set_.Scale({ 10.0f, 10.0f, 10.0f });
	worldTransform_.set_.Rotation({ Deg2Rad(0), Deg2Rad(180), Deg2Rad(180)});
}

void T::Update(float t) {
	// Update the world transform and UV transform if necessary
	// If you have any specific update logic for the ground, add it here
	Vector3 scale = worldTransform_.get_.Scale();
	scale.x = 50.0f * (1 - t) + t * 0.0f;
	worldTransform_.set_.Scale(scale);
	Vector3 uvPos = uvTransform_.get_.Translation();
	uvPos.x += 0.05f;
	uvTransform_.set_.Translation(uvPos);
	if (t <= 0.1f) {
		model_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f }); // 赤 で回避
	}
	else if (t <= 0.2f) {
		model_->SetColor({ 0.8f, 0.2f, 0.0f, 1.0f }); // 赤寄りオレンジ
	}
	else if (t <= 0.3f) {
		model_->SetColor({ 0.6f, 0.4f, 0.0f, 1.0f }); // オレンジ
	}
	else if (t <= 0.4f) {
		model_->SetColor({ 0.4f, 0.6f, 0.0f, 1.0f }); // 黄緑寄り
	}
	else if (t <= 0.5f) {
		model_->SetColor({ 0.2f, 0.8f, 0.0f, 1.0f }); // 緑寄り
	}
	else if (t <= 0.6f) {
		model_->SetColor({ 0.1f, 0.9f, 0.0f, 1.0f }); // ほぼ緑
	}
	else if (t <= 0.7f) {
		model_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑
	}
	else if (t <= 0.8f) {
		model_->SetColor({ 0.0f, 1.0f, 0.2f, 1.0f }); // 緑＋青（緑青）
	}
	else if (t <= 0.9f) {
		model_->SetColor({ 0.0f, 1.0f, 0.4f, 1.0f }); // 青寄り緑
	}
	else {
		model_->SetColor({ 0.0f, 1.0f, 0.6f, 1.0f }); // 緑青（シアン寄り）
	}

}

void T::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}