#include "DebugCamera.h"
#include "InputManager.h"
#include "ImGuiManager.h"

void DebugCamera::Initialize() {
	camera_.scale_ = { 1.0f,1.0f,1.0f };
	camera_.rotation_ = { 0.0f,0.0f,0.0f };
	camera_.translation_ = { 0.0f,1.9f,-6.49f };

	projection_.fovY = 0.45f;
	projection_.nearClip = 0.1f;
	projection_.farClip = 100.0f;
	projection_.aspectRatio = 1280.0f / 720.0f;

	speed_ = 0.1f;
	rotateSpeed_ = 0.001f;
}

void DebugCamera::UpData() {

	MoveForward();
	MoveBack();
	MoveUp();
	MoveDown();
	MoveRight();
	MoveLeft();
	RotatePitch();
	RotateYaw();
	RotateRoll();

	ImGui::Begin("Camera");
	ImGuiManager::CreateImGui("scale", camera_.scale_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("rotate", camera_.rotation_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("translate", camera_.translation_, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("fovY", projection_.fovY, 0.45f, 1.0f);
	ImGuiManager::CreateImGui("nearClip", projection_.nearClip, 0.1f, 1.0f);
	ImGuiManager::CreateImGui("farClip", projection_.farClip, 100.0f, 110.0f);
	ImGui::End();
	viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(camera_.scale_, camera_.rotation_, camera_.translation_));
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewProjectionMatrix_, Matrix4x4::Make::PerspectiveFov(projection_.fovY, projection_.aspectRatio, projection_.nearClip, projection_.farClip));
}

Matrix4x4 DebugCamera::DrawCamera(const Matrix4x4& world) {
	Matrix4x4 WVP = Matrix4x4::Multiply(world, viewProjectionMatrix_);
	return WVP;
}

void DebugCamera::MoveForward() {
	if (InputManager::GetKey().PressKey(DIK_UP)) {
		camera_.translation_.z += speed_;
	}
}

void DebugCamera::MoveBack() {
	if (InputManager::GetKey().PressKey(DIK_DOWN)) {
		camera_.translation_.z -= speed_;
	}
}

void DebugCamera::MoveRight() {
	if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
		camera_.translation_.x -= speed_;
	}
}

void DebugCamera::MoveLeft() {
	if (InputManager::GetKey().PressKey(DIK_LEFT)) {
		camera_.translation_.x += speed_;
	}
}

void DebugCamera::MoveUp() {
	if (InputManager::GetKey().PressKey(DIK_I)) {
		camera_.translation_.y -= speed_;
	}
}

void DebugCamera::MoveDown() {
	if (InputManager::GetKey().PressKey(DIK_K)) {
		camera_.translation_.y += speed_;
	}
}

void DebugCamera::RotatePitch() {
	if (InputManager::GetMouse().IsButtonPress(0) && InputManager::GetKey().PressKey(DIK_W)) {
		camera_.rotation_.x += rotateSpeed_;
	}
	if (InputManager::GetMouse().IsButtonPress(0) && InputManager::GetKey().PressKey(DIK_S)) {
		camera_.rotation_.x -= rotateSpeed_;
	}
}

void DebugCamera::RotateYaw() {
	if (InputManager::GetMouse().IsButtonPress(0) && InputManager::GetKey().PressKey(DIK_D)) {
		camera_.rotation_.z += rotateSpeed_;
	}
	if (InputManager::GetMouse().IsButtonPress(0) && InputManager::GetKey().PressKey(DIK_A)) {
		camera_.rotation_.z -= rotateSpeed_;
	}
}

void DebugCamera::RotateRoll() {
	if (InputManager::GetMouse().IsButtonPress(1) && InputManager::GetKey().PressKey(DIK_W)) {
		camera_.rotation_.y += rotateSpeed_;
	}
	if (InputManager::GetMouse().IsButtonPress(1) && InputManager::GetKey().PressKey(DIK_S)) {
		camera_.rotation_.y -= rotateSpeed_;
	}
}