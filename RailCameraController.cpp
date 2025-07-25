#include "RailCameraController.h"
#include "ImGuiManager.h"

void RailCameraController::Initialize(CameraBase* camera) {
	camera_ = camera;
	moveSpeed_ = { 0, 0, 0.1f }; // 初期移動速度を設定
}

void RailCameraController::Update() {
	Move();
}

void RailCameraController::Move() {
	Vector3 targetPos = camera_->targetPos_;

	targetPos += moveSpeed_;

	camera_->targetPos_ = targetPos;
}