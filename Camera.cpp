#include <algorithm>
#include "Camera.h"
#include "Transform.h"
#include "Player.h"
#include "ImGuiManager.h"

void Camera::Initialize() {
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

void Camera::UpData() {

	ImGui::Begin("Camera");
	ImGuiManager::CreateImGui("Translate", camera_.translation_, -10, 100);
	ImGui::End();

	if (IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Multiply(Matrix4x4::Make::Translate(camera_.translation_), camera_.matRot_));
	}
	if (!IsPivot_) {
		viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(camera_.scale_, camera_.rotation_, camera_.translation_));
	}
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewProjectionMatrix_, Matrix4x4::Make::PerspectiveFov(projection_.fovY, projection_.aspectRatio, projection_.nearClip, projection_.farClip));
}

Matrix4x4 Camera::DrawCamera(const Matrix4x4& world) {
	Matrix4x4 WVP = Matrix4x4::Multiply(world, viewProjectionMatrix_);
	return WVP;
}

void CameraController::Initialize(Camera* camera) {
	viewProjection_ = {};
	camera_ = camera;
}

void CameraController::Update() {
	// 追従対象のワールドたランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	targetPos_.x = targetWorldTransform.translate.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	targetPos_.y = targetWorldTransform.translate.y + targetOffset_.y + target_->GetVelocity().y * kVelocityBias;
	targetPos_.z = targetWorldTransform.translate.z + targetOffset_.z + target_->GetVelocity().z * kVelocityBias;

	//　座標補間によりゆったり追従
	camera_->camera_.translation_ = Lerp(camera_->camera_.translation_, targetPos_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->camera_.translation_.x = std::clamp(camera_->camera_.translation_.x, targetPos_.x + movementMargin.left, targetPos_.x + movementMargin.right);
	camera_->camera_.translation_.y = std::clamp(camera_->camera_.translation_.y, targetPos_.y + movementMargin.bottom, targetPos_.y + movementMargin.top);

	// 移動範囲制限
	camera_->camera_.translation_.x = std::clamp(camera_->camera_.translation_.x, movableArea_.left, movableArea_.right);

	camera_->camera_.translation_.y = std::clamp(camera_->camera_.translation_.y, movableArea_.top, movableArea_.bottom);

}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->camera_.translation_.x = targetWorldTransform.translate.x + targetOffset_.x;
	camera_->camera_.translation_.y = targetWorldTransform.translate.y + targetOffset_.y;
	camera_->camera_.translation_.z = targetWorldTransform.translate.z + targetOffset_.z;
}