#include "SkyDome.h"

void SkyDome::Initialize(ModelObject* model) {
	model_ = model;

	transform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
}

void SkyDome::UpDate() {
}

void SkyDome::Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}

void SkyDome::Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}