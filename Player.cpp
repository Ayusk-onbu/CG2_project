#include "Player.h"
#include "InputManager.h"

void Player::Initialize(std::unique_ptr<ModelObject>model,CameraBase* camera) {
	model_ = move(model);// 所有権の以降
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::Update() {
	//worldTransform_.set_.Rotation({0.0f,worldTransform_.get_.Rotation().y + 0.01f,0.0f});
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, worldTransform_.mat_);
	model_->Draw(command,pso,light,tex);
}
