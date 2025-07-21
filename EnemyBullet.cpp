#include "Enemy.h"
#include "ImGuiManager.h"
#include "Player.h"
#include "MathUtils.h"
#include <cassert>

void EnemyBullet::Initialize(D3D12System& d3d12, ModelObject* model, const Vector3& position, const Vector3& velocity) {
	assert(model);
	model_ = {};
	model_.Initialize(d3d12, model->GetModelData());
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(position);
	worldTransform_.set_.Scale({0.5f,0.5f,3.0f});
	velocity_ = velocity;
	/*Vector3 rotation = worldTransform_.get_.Rotation();
	rotation.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityZ = Matrix4x4::Transform(velocity_, Matrix4x4::Make::RotateY(-rotation.y));
	rotation.x = std::atan2(-velocity_.y, velocityZ.z);
	worldTransform_.set_.Rotation(rotation);*/
	Vector3 rotation = worldTransform_.get_.Rotation();
	rotation.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = Length({ velocity_.x,0.0f,velocity_.z });
	rotation.x = std::atan2(-velocity_.y, velocityXZ);
	worldTransform_.set_.Rotation(rotation);
	worldTransform_.LocalToWorld();
	bulletSpeed_ = 0.1f;
}

void EnemyBullet::Update() {
	Vector3 translation = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();

	Homing();
	translation += velocity_;

	rotation.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = Length({ velocity_.x,0.0f,velocity_.z });
	rotation.x = std::atan2(-velocity_.y, velocityXZ);
	worldTransform_.set_.Translation(translation);
	worldTransform_.set_.Rotation(rotation);
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(CameraBase& camera,
	TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	ImGui::Begin("Bullet");
	ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), -50, 50);
	ImGui::End();
	worldTransform_.LocalToWorld();
	model_.SetWVPData(camera.DrawCamera(worldTransform_.mat_), worldTransform_.mat_, Matrix4x4::Make::Identity());
	model_.Draw(command, pso, light, tex);
}

void EnemyBullet::OnCollision() {
	isDead_ = true;
}

void EnemyBullet::Homing() {
	Vector3 toPlayer = player_->GetWorldTransform().GetWorldPos() - worldTransform_.GetWorldPos();

	Normalize(toPlayer);
	Normalize(velocity_);

	velocity_ = Slerp(velocity_, toPlayer, 0.25f) * bulletSpeed_;
}

void EnemyBullet::SetTarget(const Player& player) {
	player_ = &player;
}