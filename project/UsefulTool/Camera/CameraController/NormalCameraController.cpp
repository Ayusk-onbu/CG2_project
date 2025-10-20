#include "CameraController.h"
#include "Camera.h"
#include <algorithm>
#include "Easing.h"

void NormalCameraController::Update(Camera& camera) {

	camera.GetTranslation() = camera.targetPos_;
	float theta = 0;
	camera.GetTheta() = std::clamp(camera.GetTheta(), -89.0f, 89.0f); // X軸回転は-90度から90度まで
	theta = Deg2Rad(camera.GetTheta());
	float phi = 0;
	phi = Deg2Rad(camera.GetPhi());
	// ここから変換
	camera.GetTranslation().x = camera.targetPos_.x + camera.GetRadius() * std::cos(theta) * std::cos(phi);
	camera.GetTranslation().y = camera.targetPos_.y + camera.GetRadius() * std::sin(theta);
	camera.GetTranslation().z = camera.targetPos_.z + camera.GetRadius() * std::cos(theta) * std::sin(phi);
	//viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(scale_, rotation_, translation_));
	camera.GetViewProjectionMatrix() = (Matrix4x4::Make::LookAt(camera.GetTranslation(), camera.targetPos_, camera.up_, camera.xAxis_, camera.yAxis_, camera.zAxis_));

	camera.GetViewProjectionMatrix() = Matrix4x4::Multiply(camera.GetViewProjectionMatrix(), Matrix4x4::Make::PerspectiveFov(
		camera.GetProjection().fovY, camera.GetProjection().aspectRatio,
		camera.GetProjection().nearClip, camera.GetProjection().farClip));
}