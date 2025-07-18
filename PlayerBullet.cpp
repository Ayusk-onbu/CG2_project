#include "Player.h"
#include "ImGuiManager.h"
#include <cassert>

void PlayerBullet::Initialize(D3D12System& d3d12, ModelObject*model,const Vector3&position) {
	assert(model);
	model_ = {};
	model_.Initialize(d3d12, model->GetModelData());
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(position);
}

void PlayerBullet::Update() {

	
}

void PlayerBullet::Draw(CameraBase& camera,
	TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	ImGui::Begin("Bullet");
	ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), -50, 50);
	ImGui::End();
	worldTransform_.LocalToWorld();
	model_.SetWVPData(camera.DrawCamera(worldTransform_.mat_), worldTransform_.mat_, Matrix4x4::Make::Identity());
	model_.Draw(command,pso,light,tex);
}