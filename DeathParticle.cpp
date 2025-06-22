#include "DeathParticle.h"
#include <algorithm>
#include "Math.h"

void DeathParticle::Initialize(ModelObject* model, Vector3& position) {
	for (Transform& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translate = position;
	}
	color_ = { 1, 1, 1, 1 };
	model_ = model;
}
void DeathParticle::UpDate() {

	if (isFinished_) {
		return;
	}

	counter_ += 1.0f / 60.0f;

	for (uint32_t i = 0; i < kNumParticles; ++i) {

		Vector3 velocity = { kSpeed,0.0f,0.0f };
		float angle = kAngle * float(i);
		Matrix4x4 matrixRotation = Matrix4x4::Make::RotateZ(angle);
		velocity = TransformV(velocity, matrixRotation);
		worldTransform_[i].translate.x += velocity.x;
		worldTransform_[i].translate.y += velocity.y;
		worldTransform_[i].translate.z += velocity.z;

	}

	color_.w = std::clamp(1.0f - (counter_ / kDuraction), 0.0f, 1.0f);
	model_->SetColor(color_);

	if (counter_ >= kDuraction) {

		counter_ = kDuraction;
		isFinished_ = true;
	}

}
void DeathParticle::Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (isFinished_) {
		return;
	}
	for (Transform& worldTransform : worldTransform_) {
		//ゲームの処理
		Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransform.scale, worldTransform.rotate, worldTransform.translate);
		Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
		model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
		model_->Draw(command, pso, light, tex);
	}
}

void DeathParticle::Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (isFinished_) {
		return;
	}
	for (Transform& worldTransform : worldTransform_) {
		//ゲームの処理
		Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransform.scale, worldTransform.rotate, worldTransform.translate);
		Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
		uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
		model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
		model_->Draw(command, pso, light, tex);
	}
}