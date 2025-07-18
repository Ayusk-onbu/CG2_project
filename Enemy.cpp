#include "Enemy.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "MathUtils.h"
#include <algorithm>

void Enemy::Initialize(D3D12System& d3d12, ModelObject& model, CameraBase* camera, const Vector3& pos) {
	d3d12_ = &d3d12;
	model_ = model;
	model_.Initialize(d3d12, model.GetModelData());
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation({ 0.0f,Deg2Rad(180),0.0 });
}

void Enemy::Update() {
	Vector3 pos = worldTransform_.get_.Translation();
	Vector3 rotation = { worldTransform_.get_.Rotation().x,worldTransform_.get_.Rotation().y,0.0f };
	//worldTransform_.set_.Rotation({0.0f,worldTransform_.get_.Rotation().y + 0.01f,0.0f});

	Move(pos);
	Rotate(rotation);

	ImGui::Begin("Player");

	ImGui::Text("SRT");
	if (ImGui::BeginTabBar("SRTBar")) {
		ImGuiManager::CreateImGui("Scale", worldTransform_.get_.Scale(), 0.0f, 10.0f);
		ImGuiManager::CreateImGui("Rotation", worldTransform_.get_.Rotation(), 0.0f, 180.0f);
		ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), 0.0f, 1280.0f);
		ImGui::EndTabBar();
	}

	ImGui::End();

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
}

void Enemy::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	model_.SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, Matrix4x4::Make::Identity());
	model_.Draw(command, pso, light, tex);
}

void Enemy::Move(Vector3& pos) {
	pos.z -= kMoveSpeed_;
}

void Enemy::Rotate(Vector3& rotation) {
	
}