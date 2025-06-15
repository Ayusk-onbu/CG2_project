#include "DebugCamera.h"
#include "InputManager.h"
#include "ImGuiManager.h"

void DebugCamera::Initialize() {
	camera_.scale_ = { 1.0f,1.0f,1.0f };
	camera_.rotation_ = { 0.0f,0.0f,0.0f };
	camera_.matRot_ = {
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f, 
	0.0f,0.0f,1.0f,0.0f, 
	0.0f,0.0f,0.0f,1.0f, };
	camera_.translation_ = { 0.0f,1.9f,-6.49f };

	projection_.fovY = 0.45f;
	projection_.nearClip = 0.1f;
	projection_.farClip = 100.0f;
	projection_.aspectRatio = 1280.0f / 720.0f;

	IsPivot_ = true;
	speed_ = 0.1f;
	rotateSpeed_ = { 0.0f,0.0f };
}

void DebugCamera::UpData() {

	rotateSpeed_.x = InputManager::GetMouse().getDelta().x ;
	rotateSpeed_.y = InputManager::GetMouse().getDelta().y ;

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
	ImGui::Checkbox("IsPivot", &IsPivot_);
	ImGuiManager::CreateImGui("scale", camera_.scale_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("rotate", camera_.rotation_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("matRotate", camera_.matRot_, -360.0f, 360.0f);
	ImGuiManager::CreateImGui("translate", camera_.translation_, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("fovY", projection_.fovY, 0.45f, 1.0f);
	ImGuiManager::CreateImGui("nearClip", projection_.nearClip, 0.1f, 1.0f);
	ImGui::End();

	if (IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Multiply(Matrix4x4::Make::Translate(camera_.translation_), camera_.matRot_));
	}
	if (!IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(camera_.scale_, camera_.rotation_, camera_.translation_));
	}
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewProjectionMatrix_, Matrix4x4::Make::PerspectiveFov(projection_.fovY, projection_.aspectRatio, projection_.nearClip, projection_.farClip));
}

Matrix4x4 DebugCamera::DrawCamera(const Matrix4x4& world) {
	Matrix4x4 WVP = Matrix4x4::Multiply(world, viewProjectionMatrix_);
	ImGui::Begin("WVP");
	ImGuiManager::CreateImGui("WVP", WVP, -1.0f, 10.0f);
	ImGui::End();
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
	if (InputManager::GetMouse().IsButtonPress(0)) {
		if (IsPivot_) { GetRotateDelta(rotateSpeed_.x, 0.0f); } 
		camera_.rotation_.x += rotateSpeed_.x;
	}
	if (InputManager::GetMouse().IsButtonPress(0)) {
		if (IsPivot_) { GetRotateDelta(-rotateSpeed_.x, 0.0f); }
		camera_.rotation_.x -= rotateSpeed_.x;
	}
}

void DebugCamera::RotateYaw() {
	if (InputManager::GetMouse().IsButtonPress(0)) {
		camera_.rotation_.z += InputManager::GetMouse().getScrollDelta();
	}
	if (InputManager::GetMouse().IsButtonPress(0)) {
		camera_.rotation_.z -= InputManager::GetMouse().getScrollDelta();
	}
}

void DebugCamera::RotateRoll() {
	if (InputManager::GetMouse().IsButtonPress(0)) {
		if (IsPivot_) { GetRotateDelta(0.0f,rotateSpeed_.y); }
		camera_.rotation_.y += rotateSpeed_.y;
	}
	if (InputManager::GetMouse().IsButtonPress(0)) {
		if (IsPivot_) { GetRotateDelta(0.0f,-rotateSpeed_.y); }
		camera_.rotation_.y -= rotateSpeed_.y;
	}
}