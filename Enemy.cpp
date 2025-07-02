#include "Enemy.h"
#include "Trigonometric.h"
#include "Player.h"

class Player;
void Enemy::Initialize(ModelObject* model, CameraBase* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	// textureHandle_ = textureHandle;
	camera_ = camera;

	walkTimer_ = 0.0f;
	velocity_ = { -kWalkSpeed, 0.0f, 0.0f }; // 初期速度設定
	worldTransform_ = { {1.0f, 1.0f, 1.0f},
				   {0.0f, 0.0f, 0.0f},
				   {0.0f, 0.0f, 0.0f} };
	uvTransform_ = { {1.0f, 1.0f, 1.0f},
					 {0.0f, 0.0f, 0.0f},
					 {0.0f, 0.0f, 0.0f} };
	worldTransform_.translate = position;
	worldTransform_.rotate.y = -std::numbers::pi_v<float> / 2.0f;
}

void Enemy::UpDate() {

	worldTransform_.translate = worldTransform_.translate + velocity_;

	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(2 * std::numbers::pi_v<float> *walkTimer_ / kWalkMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotate.x = Deg2Rad(radian);

}

void Enemy::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (model_) {
		Matrix4x4 world = Matrix4x4::Make::Affine(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

		Matrix4x4 uv = Matrix4x4::Make::Scale(uvTransform_.scale);
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate(uvTransform_.translate));

		model_->SetWVPData(camera_->DrawCamera(world), world, uv);
		model_->Draw(command, pso, light, tex);
	}
}

void Enemy::OnCollision(const Player* player) {
	(void)player;
}