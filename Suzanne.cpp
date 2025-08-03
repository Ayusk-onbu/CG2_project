#include "OBJ.h"
#include "ImGuiManager.h"
#include "InputManager.h"
#include <algorithm>

void Suzanne::Initialize(ModelObject& model, CameraBase& camera) {
	OBJ::Initialize(model, camera);
	worldTransform_.set_.Translation({ 0.0f,-5.0f,0.0f });
	worldTransform_.set_.Rotation({ 0.0f,Deg2Rad(180),0.0f });
	model_->SetColor(color_); // Set a default color for Suzanne
}
void Suzanne::Update() {
	Vector3 pos = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();
	Vector3 scale = worldTransform_.get_.Scale();
	Vector3 uvPos = uvWorldTransform_.get_.Translation();
	Vector3 uvRotation = uvWorldTransform_.get_.Rotation();
	Vector3 uvScale = uvWorldTransform_.get_.Scale();
	SHORT lx = 0;
	SHORT ly = 0;
	if (InputManager::GetGamePad(0).IsConnected()) {
		lx = InputManager::GetGamePad(0).GetLeftStickX();
		ly = InputManager::GetGamePad(0).GetLeftStickY();
	}
	if (lx != 0 || ly != 0) {
		pos.x += std::clamp(static_cast<float>(lx), -32767.0f, 32767.0f) / 32767 * 0.1f;
		pos.y += std::clamp(static_cast<float>(ly), -32767.0f, 32767.0f) / 32767 * 0.1f;
	}
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_A)) {
		rotation.x += Deg2Rad(1.0f);
	}
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_Y)) {
		rotation.x -= Deg2Rad(1.0f);
	}
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_X)) {
		rotation.y += Deg2Rad(1.0f);
	}
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_B)) {
		rotation.y -= Deg2Rad(1.0f);
	}

	ImGui::Begin("Suzanne");
	ImGuiManager::CreateImGui("Position", pos, -32.0f, 32.0f);
	ImGuiManager::CreateImGui("Rotation", rotation, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("Scale", scale, 0.1f, 5.0f);
	ImGuiManager::CreateImGui("UV Position", uvPos, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("UV Rotation", uvRotation, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("UV Scale", uvScale, 0.1f, 5.0f);
	ImGui::ColorEdit4("Color", (float*)&color_); // Allow color editing for Suzanne
	ImGui::End();

	model_->SetColor(color_);
	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
	worldTransform_.set_.Scale(scale);
	uvWorldTransform_.set_.Translation(uvPos);
	uvWorldTransform_.set_.Rotation(uvRotation);
	uvWorldTransform_.set_.Scale(uvScale);
}
void Suzanne::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	OBJ::Draw(command, pso, light, tex);
}