#include "Enemy.h"
#include <cassert>

float Dig2Radian(float degree) {
	return degree * (std::numbers::pi_v<float> / 180.0f);
}

void Enemy::Initialize(ModelObject* model, Vector3& pos) {
	assert(model);
	model_ = model;
	// textureHandle_ = textureHandle;

	walkTimer_ = 0.0f;
	velocity_ = { -kWalkSpeed, 0.0f, 0.0f }; // 初期速度設定
	worldTransform_.Initialize();
	worldTransform_.translate = pos;
	worldTransform_.rotate.y = -std::numbers::pi_v<float> / 2.0f;

	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
}

void Enemy::UpDate() {

	worldTransform_.translate = worldTransform_.translate + velocity_;



	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(2 * std::numbers::pi_v<float> *walkTimer_ / kWalkMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotate.x = Dig2Radian(radian);

}

void Enemy::Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}

void Enemy::Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//ゲームの処理
	Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale(uvTransform_.scale);
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
	uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate(uvTransform_.translate));
	model_->SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
	model_->Draw(command, pso, light, tex);
}

void Enemy::OnCollision(const Player* player) {
	(void)player;
}