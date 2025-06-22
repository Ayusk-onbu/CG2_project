#include "Text.h"
#include "Math.h"

void Text::Initialize(ModelObject* model) {
	model_ = model;

	transform_ = {
		{10.0f,10.0f,10.0f},
		{0.0f,90.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	rad_ = 0.0;
}

void Text::UpDate() {

	rad_ += 1;
	transform_.translate.y = std::sin(rad_ / 10.0f);

}

void Text::Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}

void Text::Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}