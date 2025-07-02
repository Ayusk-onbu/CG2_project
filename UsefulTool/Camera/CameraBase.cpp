#include "CameraBase.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include <algorithm>

void CameraBase::Initialize() {
	camera_.scale_ = { 1.0f,1.0f,1.0f };
	camera_.rotation_ = { 0.0f,0.0f,0.0f };
	camera_.matRot_ = {
	1.0f,0.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,0.0f,
	0.0f,0.0f,1.0f,0.0f,
	0.0f,0.0f,0.0f,1.0f, };
	camera_.translation_ = { 0.0f,10.0f,50.0f }; // カメラの位置
	targetPos_ = { 0.0f,0.0f,10.0f };
	phi_ = 270.0f; // Y軸回転
	theta_ = 0.0f; // X軸回転
	radius_ = 50.0f;
	xAxis_ = { 1.0f,0.0f,0.0f }; // X軸
	yAxis_ = { 0.0f,1.0f,0.0f }; // Y軸
	zAxis_ = { 0.0f,0.0f,1.0f }; // Z軸

	projection_.fovY = 0.45f;
	projection_.nearClip = 0.1f;
	projection_.farClip = 100.0f;
	projection_.aspectRatio = 1280.0f / 720.0f;
}

void CameraBase::UpDate() {
	float panSpeed = 0.005f * radius_;
	//   Zoom
	// マウスホイールで操作
	radius_ -= InputManager::GetMouse().GetWheel() * panSpeed * 0.1f;
	//targetPos_.z -= InputManager::GetMouse().GetWheel() * 0.001f;
	//   視点の移動
	// Shift + マウスの真ん中
	
	if (InputManager::GetKey().PressKey(DIK_LSHIFT) && InputManager::GetMouse().IsButtonPress(2)) {
		//targetPos_.x -= InputManager::GetMouse().getDelta().x * panSpeed;
		//targetPos_.y += InputManager::GetMouse().getDelta().y * panSpeed; // Y軸は反転
		Vector3 right = CalculateRight();
		Vector3 up = CalculateUp();
		targetPos_.x -= right.x * InputManager::GetMouse().getDelta().x * panSpeed;
		targetPos_.x += up.x * InputManager::GetMouse().getDelta().y * panSpeed; // Y軸は反転
		targetPos_.y -= right.y * InputManager::GetMouse().getDelta().x * panSpeed;
		targetPos_.y += up.y * InputManager::GetMouse().getDelta().y * panSpeed; // Y軸は反転
		targetPos_.z -= right.z * InputManager::GetMouse().getDelta().x * panSpeed;
		targetPos_.z += up.z * InputManager::GetMouse().getDelta().y * panSpeed; // Y軸は反転
	}

	//   視点の回転
	// マウスの中ボタンのみ
	if (InputManager::GetMouse().IsButtonPress(2) && !InputManager::GetKey().PressKey(DIK_LSHIFT)) {
		theta_ +=(InputManager::GetMouse().getDelta().y * 0.1f);
		phi_ -= (InputManager::GetMouse().getDelta().x * 0.1f);
	}
	ImGui::Begin("CameraBase");
	ImGuiManager::CreateImGui("translation", camera_.translation_, -5.0f, 5.0f);
	ImGuiManager::CreateImGui("rotation", camera_.rotation_, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("phi", phi_, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("theta", theta_, -180.0f, 180.0f);
	ImGuiManager::CreateImGui("xAxis", xAxis_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("yAxis", yAxis_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("zAxis", zAxis_, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("Target.translation", targetPos_, -5.0f, 5.0f);
	ImGui::End();
	float theta = 0;
	theta_ = std::clamp(theta_, -89.0f, 89.0f); // X軸回転は-90度から90度まで
	theta = Deg2Rad(theta_);
	float phi = 0;
	phi = Deg2Rad(phi_);
	
	
	// ここから変換
	camera_.translation_.x = targetPos_.x + radius_ * std::cos(theta) * std::cos(phi);
	camera_.translation_.y = targetPos_.y + radius_ * std::sin(theta);
	camera_.translation_.z = targetPos_.z + radius_ * std::cos(theta) * std::sin(phi);
	//viewProjectionMatrix_ = Matrix4x4::Inverse(Matrix4x4::Make::Affine(camera_.scale_, camera_.rotation_, camera_.translation_));
	viewProjectionMatrix_ = (Matrix4x4::Make::LookAt(camera_.translation_, targetPos_, { 0.0f,1.0f,0.0f },xAxis_,yAxis_,zAxis_));
	viewProjectionMatrix_ = Matrix4x4::Multiply(viewProjectionMatrix_, Matrix4x4::Make::PerspectiveFov(
							projection_.fovY, projection_.aspectRatio, 
							projection_.nearClip, projection_.farClip));
}

Matrix4x4 CameraBase::DrawCamera(const Matrix4x4& world) {
	Matrix4x4 WVP = Matrix4x4::Multiply(world, viewProjectionMatrix_);
	return WVP;
}

Vector3 CameraBase::CalculateRight() {
	return Normalize(CrossProduct({ 0.0f,1.0f,0.0f }, Normalize(Subtract(targetPos_, camera_.translation_))));
}
Vector3 CameraBase::CalculateUp() {
	Vector3 forward = Normalize(Subtract(targetPos_, camera_.translation_));
	Vector3 right = CalculateRight();
	return Normalize(CrossProduct(forward, right));
}

void CameraBase::SetTargetPos(Vector3& target) {
	targetPos_ = target;
}