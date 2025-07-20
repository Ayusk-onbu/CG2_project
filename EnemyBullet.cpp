#include "Enemy.h"
#include "ImGuiManager.h"
#include <cassert>

void EnemyBullet::Initialize(D3D12System& d3d12, ModelObject* model, const Vector3& position, const Vector3& velocity) {
	assert(model);
	model_ = {};
	model_.Initialize(d3d12, model->GetModelData());
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(position);
	velocity_ = velocity;
}

void EnemyBullet::Update() {
	Vector3 translation = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();

	translation += velocity_;
	rotation.y += Deg2Rad(90);

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