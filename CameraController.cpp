#include "CameraController.h"
#include "Transform.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize(CameraBase* camera) {
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
	camera_->targetPos_ = Lerp(camera_->targetPos_, targetPos_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->targetPos_.x = std::clamp(camera_->targetPos_.x, targetPos_.x + movementMargin.left, targetPos_.x + movementMargin.right);
	camera_->targetPos_.y = std::clamp(camera_->targetPos_.y, targetPos_.y + movementMargin.bottom, targetPos_.y + movementMargin.top);

	// 移動範囲制限
	camera_->targetPos_.x = std::clamp(camera_->targetPos_.x, movableArea_.left, movableArea_.right);
	camera_->targetPos_.y = std::clamp(camera_->targetPos_.y, movableArea_.bottom, movableArea_.top);

}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->targetPos_.x = targetWorldTransform.translate.x + targetOffset_.x;
	camera_->targetPos_.y = targetWorldTransform.translate.y + targetOffset_.y;
	camera_->targetPos_.z = targetWorldTransform.translate.z + targetOffset_.z;
}
