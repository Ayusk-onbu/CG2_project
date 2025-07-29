#include "Player.h"
#include "ImGuiManager.h"
#include "Enemy.h"
#include "MathUtils.h"
#include <cassert>

void PlayerBulletHoming::Initialize(D3D12System& d3d12, ModelObject* model, const Vector3& position, const Vector3& velocity) {
	assert(model);
	model_ = {};
	model_.Initialize(d3d12, model->GetModelData());
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(position);
	worldTransform_.set_.Scale({ 0.25f,0.25f,1.5f });
	velocity_ = velocity;
	Vector3 rotation = worldTransform_.get_.Rotation();
	rotation.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = Length({ velocity_.x,0.0f,velocity_.z });
	rotation.x = std::atan2(-velocity_.y, velocityXZ);
	worldTransform_.set_.Rotation(rotation);
	worldTransform_.LocalToWorld();

	SetMyType(0b1);
	SetYourType(~0b1);
}

void PlayerBulletHoming::Update() {
	Vector3 translation = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();
	
	translation += velocity_;
	rotation.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = Length({ velocity_.x,0.0f,velocity_.z });
	rotation.x = std::atan2(-velocity_.y, velocityXZ);
	Homing();
	worldTransform_.set_.Translation(translation);
	worldTransform_.set_.Rotation(rotation);
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBulletHoming::Draw(CameraBase& camera,
	TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
#ifdef _DEBUG
	ImGui::Begin("BulletHoming");
	ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), -50, 50);
	ImGuiManager::CreateImGui("Velocity", velocity_, 0.0f, 1.0f);
	ImGui::End();
#endif // DEBUG
	worldTransform_.LocalToWorld();
	model_.SetWVPData(camera.DrawCamera(worldTransform_.mat_), worldTransform_.mat_, Matrix4x4::Make::Identity());
	model_.Draw(command, pso, light, tex);
}

void PlayerBulletHoming::OnCollision() {
	isDead_ = true;
}

void PlayerBulletHoming::Homing() {
	if(!targetEnemy_ || targetEnemy_->IsDead()) {
		return; // ターゲットが設定されていない場合は何もしない
	}
	Vector3 toPlayer = targetEnemy_->GetWorldTransform().GetWorldPos() - worldTransform_.GetWorldPos();

	Normalize(toPlayer);
	Normalize(velocity_);

	velocity_ = Slerp(velocity_, toPlayer, 0.25f) * 0.1f;
}

void PlayerBulletHoming::SetTarget(const Enemy& player) {
	targetEnemy_ = &player;
}