#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include <algorithm>

void Player::Initialize(std::unique_ptr<ModelObject>model,CameraBase* camera) {
	model_ = move(model);// 所有権の以降
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::Update() {
	Vector3 pos = worldTransform_.get_.Translation();
	float rotation = 0.0f;
	//worldTransform_.set_.Rotation({0.0f,worldTransform_.get_.Rotation().y + 0.01f,0.0f});
	
	Move(pos, rotation);

	ImGui::Begin("Player");

	ImGui::Text("SRT");
	if (ImGui::BeginTabBar("SRTBar")) { 
		ImGuiManager::CreateImGui("Scale", worldTransform_.get_.Scale(), 0.0f, 10.0f);
		ImGuiManager::CreateImGui("Rotation", worldTransform_.get_.Rotation(), 0.0f, 180.0f);
		ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), 0.0f, 1280.0f);
		ImGui::EndTabBar();
	}
	
	ImGui::End();

	pos.x = std::clamp(pos.x, -kMoveLimitX_.x, kMoveLimitX_.x);
	pos.y = std::clamp(pos.y, -kMoveLimitX_.y, kMoveLimitX_.y);

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation({ 0.0f,0.0f,rotation });
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_,Matrix4x4::Make::Identity());
	model_->Draw(command,pso,light,tex);
}

void Player::Move(Vector3& pos, float& rotation) {
	if (InputManager::GetKey().PressKey(DIK_LEFT)) {
		pos.x -= kMoveSpeed_;
		rotation = -75.0f;
	}
	else if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
		pos.x += kMoveSpeed_;
		rotation = 75.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_DOWN)) {
		pos.y -= kMoveSpeed_;
	}
	else if (InputManager::GetKey().PressKey(DIK_UP)) {
		pos.y += kMoveSpeed_;
	}
}
