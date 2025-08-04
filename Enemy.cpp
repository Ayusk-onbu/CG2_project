#include "Enemy.h"
#include "Player.h"
#include "InputManager.h"
#include <algorithm>

void Enemy::Initialize(ModelObject* model, CameraBase* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation({ 10.0f, 1.0f, 0.0f });
	worldTransform_.set_.Scale({ 1.0f, 1.0f, 1.0f });
	uvTransform_.Initialize();

	SetMyType(0b1 << 0);
	SetYourType(0b1 << 1);
}

void Enemy::Update(const Player& player) {
	Vector3 position = worldTransform_.get_.Translation();
	Vector3 scale = worldTransform_.get_.Scale();

	Move(position,player);

	worldTransform_.set_.Translation(position);
	worldTransform_.set_.Scale(scale);
}

void Enemy::Move(Vector3& pos, const Player& player) {
	
	direction_ = player.GetWorldPosition() - worldTransform_.GetWorldPos();
	direction_ = Normalize(direction_);
	speed_ += acceleration_;
	speed_ = std::clamp(speed_, -0.0875f, 0.0875f);
	if (Length(direction_) > 0.1f) {
		pos.x += direction_.x * speed_;
	}
}

void Enemy::OnCollision() {

}

const Vector3 Enemy::GetWorldPosition() {
	return worldTransform_.GetWorldPos();
}

void Enemy::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, uvTransform_.mat_);
	model_->Draw(command, pso, light, tex);
}