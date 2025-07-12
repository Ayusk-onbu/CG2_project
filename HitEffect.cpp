#include "HitEffect.h"
#include "RandomUtils.h"
#include <numbers>
#include <algorithm>

ModelObject* HitEffect::model_ = nullptr;
ModelObject* HitEffect::model2_ = nullptr;
ModelObject* HitEffect::model3_ = nullptr;
CameraBase* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(Vector3& pos) {
	transform_ = {
		.scale = {0.5f,0.5f,0.5f},
		.rotate = {0.0f,Deg2Rad(180),0.0f},
		.translate = {0.0f,0.0f,0.0f}
	};
	color_ = { 1.0f,1.0f,1.0f,1.0f };
	transform_.translate = pos;
	transform_.translate.y += 0.5f;
	behaviorRequest_ = BEHAVIOR::Expand;
	isDraw_ = true;
	rotate2_ = Deg2Rad(static_cast<float>(TimeRandom::GetRandom(360) - 180));
	rotate3_ = Deg2Rad(static_cast<float>(TimeRandom::GetRandom(360) - 180));
}

void HitEffect::Update() {
	BehaviorTransition();
	BehaviorUpdate();
}

void HitEffect::BehaviorUpdate() {
	switch (behavior_) {
	case BEHAVIOR::Expand:
		BehaviorExpandUpdate();
		break;
	case BEHAVIOR::FadeOut:
		BehaviorFadeOutUpDate();
		break;
	case BEHAVIOR::Destroy:
		BehaviorDestroyUpdate();
		break;
	}
}

void HitEffect::BehaviorExpandUpdate() {
	if (counter_ >= 0.5f) {
		behaviorRequest_ = BEHAVIOR::FadeOut;
	}
	counter_ += 1.0f / 60.0f;
	float x = counter_ / 0.5f;
	x = 1.0f - std::powf((1.0f - x), 2.0f);
	transform_.scale.x = 0.5f + (0.6f - 0.5f) * x;
	transform_.scale.y = transform_.scale.x;
}

void HitEffect::BehaviorFadeOutUpDate() {
	if (counter_ >= 0.5f) {
		behaviorRequest_ = BEHAVIOR::Destroy;
	}

	counter_ += 1.0f / 60.0f;
	color_.w = std::clamp(1.0f - (counter_ / 0.5f), 0.0f, 1.0f);
	model_->SetColor(color_);
	color_.w = std::clamp(1.0f - (counter_ / 0.5f), 0.3f, 1.0f);
	model2_->SetColor(color_);
	model3_->SetColor(color_);
}

void HitEffect::BehaviorDestroyUpdate() {
	counter_ += 1.0f / 60.0f;
	if (counter_ >= 0.01f) {
		isDraw_ = false;
	}
}

void HitEffect::BehaviorTransition() {
	if (behaviorRequest_ == BEHAVIOR::UnKnown) {
		return;
	}
	behavior_ = behaviorRequest_;
	switch (behavior_) {
	case BEHAVIOR::Expand:
		
		break;
	case BEHAVIOR::FadeOut:
		
		break;
	case BEHAVIOR::Destroy:
		
		break;
	}
	counter_ = 0.0f;
	behaviorRequest_ = BEHAVIOR::UnKnown;
}

void HitEffect::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (isDraw_) {
		if (model_) {
			Matrix4x4 world = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
			Matrix4x4 uv = Matrix4x4::Make::Scale({1.0f,1.0f,1.0f});
			uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(0.0f));
			uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate({0.0f,0.0f,0.0f}));
			model_->SetWVPData(camera_->DrawCamera(world), world,uv);
			world = Matrix4x4::Make::Affine({ 0.05f,0.7f,0.5f }, { 0.0f,Deg2Rad(180),rotate2_ }, transform_.translate);
			model2_->SetWVPData(camera_->DrawCamera(world), world, uv);
			world = Matrix4x4::Make::Affine({ 0.05f,0.7f,0.5f }, { 0.0f,Deg2Rad(180),rotate3_ }, transform_.translate);
			model3_->SetWVPData(camera_->DrawCamera(world), world, uv);
			model_->Draw(command, pso, light, tex);
			model2_->Draw(command, pso, light, tex);
			model3_->Draw(command, pso, light, tex);
		}
	}
}