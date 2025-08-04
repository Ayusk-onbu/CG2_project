#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "MathUtils.h"

void Player::Initialize(ModelObject* model, CameraBase* camera,Enemy&enemy) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation({ 0.0f, 2.0f, 0.0f });
	worldTransform_.set_.Scale({ 1.0f, 1.0f, 1.0f });
	worldTransform_.LocalToWorld();
	uvTransform_.Initialize();
	hp_ = maxHp_ = 100;
	enemy_ = &enemy;

	SetMyType(0b1 << 1);
	SetYourType(0b1 << 0);
}

void Player::Update() {
	Vector3 position = worldTransform_.get_.Translation();
	Vector3 scale = worldTransform_.get_.Scale();

	Move(position);
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
	ImGui::Text("HP: %d", hp_);
	ImGui::Text("enemyDirection: %f", enemy_->GetDirection());
	ImGui::End();
#endif//DEBUG
	worldTransform_.set_.Translation(position);
	worldTransform_.set_.Scale(scale);
}

void Player::Move(Vector3& pos) {
	if (InputManager::GetKey().PressKey(DIK_A)) {
		pos.x -= 0.1f;
	}
	if (InputManager::GetKey().PressKey(DIK_D)) {
		pos.x += 0.1f;
	}
	Jump(pos);
}

void Player::Jump(Vector3& pos) {
	if (InputManager::GetKey().PressKey(DIK_SPACE)) {
		if (!IsJump()) {
			isJump_ = true;
			jumpState_ = 1;
		}
	}
	
	switch (jumpState_) {
	case 0:
		jumpCount_ = 0.0f; // Reset jump count when starting a jump
		break;
	case 1:
		jumpCount_ += 0.01f;
		pos = Lerp(Vector3(pos.x, 2.0f, pos.z), Vector3(pos.x, 8.0f, pos.z), jumpCount_ / jumpMaxCount_);
		if (jumpCount_ >= jumpMaxCount_) {
			jumpCount_ = 0.0f;
			jumpState_ = 2;
		}
		break;
	case 2:
		jumpCount_ += 0.01f;
		pos = Lerp(Vector3(pos.x, 8.0f, pos.z), Vector3(pos.x, 2.0f, pos.z), jumpCount_ / jumpMaxCount_);
		if (jumpCount_ >= jumpMaxCount_) {
			jumpCount_ = 0.0f;
			jumpState_ = 0;
			pos.y = 2.0f;
			isJump_ = false; // Reset jump state
		}
		break;
	}
	
	
}

void Player::OnCollision() {
	hp_ -= 5;
	Vector3 position = worldTransform_.get_.Translation();
	float direction = enemy_->GetDirection().x;
	position.x += 2.5f * direction; // Reset Y position on collision
	worldTransform_.set_.Translation(position); // Reset position on collision
}

const Vector3 Player::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}

const Vector3 Player::GetWorldPosition() const{
	return worldTransform_.GetWorldPos();
}

const bool Player::IsJump()const {
	if (isJump_) {
		return true;
	}
	return false;
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}