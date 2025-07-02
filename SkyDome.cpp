#include "SkyDome.h"
#include "Matrix4x4.h"

void SkyDome::Initialize(ModelObject& model, CameraBase& camera) {
	model_ = &model;
	camera_ = &camera;
	transform_ = { {1.0f, 1.0f, 1.0f},
				   {0.0f, 0.0f, 0.0f},
				   {0.0f, 0.0f, 0.0f} };
	uvTransform_ = { {1.0f, 1.0f, 1.0f},
					 {0.0f, 0.0f, 0.0f},
					 {0.0f, 0.0f, 0.0f} };
}

void SkyDome::Update() {
	// Update player logic here, e.g., position, rotation, etc.
	// For example, you might want to update the transform_ based on input or game logic.
}

void SkyDome::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (model_) {
		Matrix4x4 world = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);

		Matrix4x4 uv = Matrix4x4::Make::Scale(uvTransform_.scale);
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate(uvTransform_.translate));

		model_->SetWVPData(camera_->DrawCamera(world), world, uv);
		model_->Draw(command, pso, light, tex);
	}
}