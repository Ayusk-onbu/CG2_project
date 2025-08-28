#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "MathUtils.h"

void Player::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation({ 0.0f, 1.0f, 0.0f });
	worldTransform_.set_.Scale({ 1.0f, 1.0f, 1.0f });
	worldTransform_.set_.Rotation({ 0.0f,Deg2Rad(90),0.0f});
	worldTransform_.LocalToWorld();
	uvTransform_.Initialize();
	//enemy_ = &enemy;
	
	state_ = new PlayerStateStop();
	state_->GetPlayer(*this);

	stats_.Initialize();

	SetMyType(0b1 << 1);
	SetYourType(0b1 << 0);
}

void Player::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.set_.Translation({ 0.0f, 1.0f, 0.0f });
	worldTransform_.set_.Scale({ 1.0f, 1.0f, 1.0f });
	worldTransform_.set_.Rotation({ 0.0f, Deg2Rad(90), 0.0f });
	worldTransform_.LocalToWorld();
	uvTransform_.Initialize();
	//enemy_ = &enemy;

	state_ = new PlayerStateStop();
	state_->GetPlayer(*this);

	stats_.Initialize();

	SetMyType(0b1 << 1);
	SetYourType(0b1 << 0);
}

void Player::Update() {
	if (stats_.GetHp() <= 0.0f) {
		isAlive_ = false;
	}
	else {
		state_->Update();
	}
	if (isAlive_) {
		model_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
	}
	else {
		model_->SetColor({ 0.1f,0.1f,0.1f,1.0f });
	}
	/*if (isRolling_) {
		ImGui::Begin("IsRolling");
		ImGui::Text("TRUE");
		ImGui::End();
	}*/
}

void Player::OnCollision() {
	Vector3 position = worldTransform_.get_.Translation();
	if (!isRolling_) {
		float direction = enemy_->GetDirection().x;
		position.x += 2.5f * direction; // Reset Y position on collision
		stats_.SetHp(stats_.GetHp() - 0.1f);
	}
	worldTransform_.set_.Translation(position); // Reset position on collision
}

void Player::ChangeState(PlayerState* state) {
	if (state_) {
		delete state_;
	}
	state_ = state;
	state_->Initialize();
	state_->GetPlayer(*this);
}

const Vector3 Player::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}

const Vector3 Player::GetWorldPosition() const{
	return worldTransform_.GetWorldPos();
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}