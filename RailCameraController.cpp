#include "RailCameraController.h"
#include "ImGuiManager.h"
#include "MathUtils.h"
#include <algorithm>
void RailCameraController::Initialize(CameraBase* camera) {
	camera_ = camera;
	moveSpeed_ = { 0, 0, 0.1f }; // 初期移動速度を設定
	time_ = 60 * 10;
	timer_ = 0;
}

void RailCameraController::Update() {
	Move();
	timer_++;
}

void RailCameraController::Move() {
	if (timer_ > time_) {
		return;
	}
	Vector3 targetPos = camera_->targetPos_;
	float t = static_cast<float>(timer_) / static_cast<float>(time_);
	t = std::clamp(t, 0.0f, 1.0f); // tを0から1の範囲に制限
	
	targetPos = CatmullRomPosition(*railPoints_, t);

	Vector3 forward = Normalize(targetPos - preTarget_);
	if (Length(forward) < 0.01f) {
		return; // 前回の位置とほぼ同じなら何もしない
	}
	if (forward.x <= 0) {
		forward.x = 0.01f; // Y軸の値が0以下の場合、少し上に調整
	}
	float theta = std::asin(forward.y);
	float phi = std::atan2(forward.z, forward.x);
	theta = Rad2Deg(theta); // ラジアンから度に変換
	theta = std::clamp(theta, -89.0f, 89.0f); // θを-89から89の範囲に制限
	phi = Rad2Deg(phi) + 180; // ラジアンから度に変換
	//phi = std::fmod(phi + 360.0f, 360.0f); // φを0から360の範囲に制限
#ifdef _DEBUG
	ImGui::Begin("RailCameraController");
	ImGuiManager::CreateImGui("Target Position", targetPos, -100.0f, 100.0f);
	ImGuiManager::CreateImGui("Theta", theta, -89.0f, 89.0f);
	ImGuiManager::CreateImGui("Phi", phi, 0.0f, 360.0f);
	ImGuiManager::CreateImGui("forward", forward, -100.0f, 100.0f);
	ImGui::End();
#endif// DEBUG
	camera_->targetPos_ = targetPos;
	camera_->SetTheta(-theta); // ラジアンから度に変換
	camera_->SetPhi(phi); // ラジアンから度に変換
	
	preTarget_ = targetPos;
}