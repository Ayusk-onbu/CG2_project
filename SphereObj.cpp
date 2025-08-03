#include "SphereObj.h"
#include "ImGuiManager.h"
void SphereObj::Initialize(SphereObject& model, CameraBase& camera) {
	model_ = &model;
	camera_ = &camera;
	worldTransform_.Initialize();
	uvWorldTransform_.Initialize();
}
void SphereObj::Update() {
	Vector3 pos = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();
	Vector3 scale = worldTransform_.get_.Scale();
	Vector3 uvPos = uvWorldTransform_.get_.Translation();
	Vector3 uvRotation = uvWorldTransform_.get_.Rotation();
	Vector3 uvScale = uvWorldTransform_.get_.Scale();

	ImGui::Begin("Sphere");
	ImGuiManager::CreateImGui("Position", pos, -32.0f, 32.0f);
	ImGuiManager::CreateImGui("Rotation", rotation, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("Scale", scale, 0.1f, 5.0f);
	ImGuiManager::CreateImGui("UV Position", uvPos, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("UV Rotation", uvRotation, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("UV Scale", uvScale, 0.1f, 5.0f);
	ImGui::End();

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
	worldTransform_.set_.Scale(scale);
	uvWorldTransform_.set_.Translation(uvPos);
	uvWorldTransform_.set_.Rotation(uvRotation);
	uvWorldTransform_.set_.Scale(uvScale);
}
void SphereObj::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvWorldTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvWorldTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}