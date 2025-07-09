#include "DeathParticle.h"
#include <algorithm>
#include "Matrix4x4.h"


void DeathParticle::Initialize(const Vector3& position) {
	for (Transform& worldTransform : worldTransform_) {
		worldTransform.scale = { 1.0f, 1.0f, 1.0f };
		worldTransform.rotate = { 0.0f, 0.0f, 0.0f };
		worldTransform.translate = position;
	}
	uvTransform_ = { {1.0f, 1.0f, 1.0f},
					 {0.0f, 0.0f, 0.0f},
					 {0.0f, 0.0f, 0.0f} };
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	isFinished_ = true;
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
		velocity = Matrix4x4::Transform(velocity, matrixRotation);
		worldTransform_[i].translate.x += velocity.x;
		worldTransform_[i].translate.y += velocity.y;
		worldTransform_[i].translate.z += velocity.z;

	}

	color_.w = std::clamp(1.0f - (counter_ / kDuraction), 0.0f, 1.0f);
	if (counter_ >= kDuraction) {

		counter_ = kDuraction;
		isFinished_ = true;
	}

}
//void DeathParticle::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
//	if (isFinished_) {
//		return;
//	}
//	Matrix4x4 uv = Matrix4x4::Make::Scale(uvTransform_.scale);
//	uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
//	uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate(uvTransform_.translate));
//	int i = 0;
//	for (Transform& worldTransform : worldTransform_) {
//		Matrix4x4 world = Matrix4x4::Make::Affine(worldTransform.scale, worldTransform.rotate, worldTransform.translate);
//
//		model_[i]->SetWVPData(camera_->DrawCamera(world), world, uv);
//		model_[i]->Draw(command, pso, light, tex);
//	}
//}

void DeathParticle::SetPosition(const Vector3& position) {
	if (isFinished_) {
		for (Transform& worldTransform : worldTransform_) {

			worldTransform.translate = position;
		}
		if(counter_ != kDuraction)
		isFinished_ = false;
	}
}

Matrix4x4 DeathParticle::GetPosition(int i){
	Matrix4x4 world = Matrix4x4::Make::Affine(worldTransform_[i].scale, worldTransform_[i].rotate, worldTransform_[i].translate);
	return world;
}