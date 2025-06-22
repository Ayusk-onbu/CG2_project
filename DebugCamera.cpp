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
	camera_.translation_ = { 0.0f,0.0f,-16.49f };

	projection_.fovY = 0.45f;
	projection_.nearClip = 0.1f;
	projection_.farClip = 100.0f;
	projection_.aspectRatio = 1280.0f / 720.0f;

	IsPivot_ = true;
	speed_ = 0.1f;
	rotateSpeed_ = { 0.0f,0.0f };
	sensitivity_ = 0.01f;
}

void DebugCamera::UpData() {

	rotateSpeed_.x = InputManager::GetMouse().getDelta().x * sensitivity_;
	rotateSpeed_.y = InputManager::GetMouse().getDelta().y * sensitivity_ * 0.5f;

	Zoom();


	MoveUp();
	MoveDown();
	MoveRight();
	MoveLeft();
	RotatePitch();
	RotateYaw();
	RotateRoll();

	/*ImGui::Begin("Camera");
	ImGui::Checkbox("IsPivot", &IsPivot_);
	ImGuiManager::CreateImGui("scale", camera_.scale_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("rotate", camera_.rotation_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("matRotate", camera_.matRot_, -360.0f, 360.0f);
	ImGuiManager::CreateImGui("translate", camera_.translation_, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("fovY", projection_.fovY, 0.45f, 1.0f);
	ImGuiManager::CreateImGui("nearClip", projection_.nearClip, 0.1f, 1.0f);

	ImGuiManager::CreateImGui("sensitivity", sensitivity_, -1.0f, 1.0f);
	ImGui::End();*/

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
	/*ImGui::Begin("WVP");
	ImGuiManager::CreateImGui("WVP", WVP, -1.0f, 10.0f);
	ImGui::End();*/
	return WVP;
}

void DebugCamera::Zoom() {
	camera_.translation_.z += InputManager::GetMouse().GetWheel() * 0.01f;
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
	if (InputManager::GetKey().PressKey(DIK_UP)) {
		camera_.translation_.y -= speed_;
	}
}

void DebugCamera::MoveDown() {
	if (InputManager::GetKey().PressKey(DIK_DOWN)) {
		camera_.translation_.y += speed_;
	}
}

void DebugCamera::RotatePitch() {
	if (InputManager::GetMouse().IsButtonPress(2)) {
		if (IsPivot_) { GetRotateDelta(rotateSpeed_.y, 0.0f); }
		if (!IsPivot_) { camera_.rotation_.x -= rotateSpeed_.y; }
	}
}

void DebugCamera::RotateRoll() {
	if (InputManager::GetMouse().IsButtonPress(2)) {
		if (IsPivot_) { GetRotateDelta(0.0f,rotateSpeed_.x); }
		if (!IsPivot_) { camera_.rotation_.y += rotateSpeed_.x; }
	}
}


void DebugCamera::RotateYaw() {
	if (InputManager::GetMouse().IsButtonPress(0)) {
		camera_.rotation_.z += rotateSpeed_.x;
	}
}