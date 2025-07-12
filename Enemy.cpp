#include "Enemy.h"
#include "Trigonometric.h"
#include "Player.h"
#include "ImGuiManager.h"

void Enemy::Initialize(ModelObject* model, CameraBase* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	// textureHandle_ = textureHandle;
	camera_ = camera;

	walkTimer_ = 0.0f;
	velocity_ = { -kWalkSpeed, 0.0f, 0.0f }; // 初期速度設定
	worldTransform_ = { {1.0f, 1.0f, 1.0f},
				   {0.0f, 0.0f, 0.0f},
				   {0.0f, 0.0f, 0.0f} };
	uvTransform_ = { {1.0f, 1.0f, 1.0f},
					 {0.0f, 0.0f, 0.0f},
					 {0.0f, 0.0f, 0.0f} };
	worldTransform_.translate = position;
	worldTransform_.rotate.y = -std::numbers::pi_v<float> / 2.0f;
	deathFlag_ = false;
	deathTime_ = 30.0f;
	deathTimer_ = 0.0f;
}

void Enemy::UpDate() {
	BehaviorTransition();
	BehaviorUpdate();
}

void Enemy::BehaviorUpdate() {
	switch (behavior_) {
	case BEHAVIOR::Walk:
		//更新処理
		BehaviorWalkUpdate();
		break;
	case BEHAVIOR::Death:
		//更新処理
		BehaviorDeathUpdate();
		break;
	}
	ImGui::Begin("BEHAVIOR");
	ImGui::Text("BEHAVIOR : %d", behavior_);
	ImGui::End();
}

void Enemy::BehaviorWalkUpdate() {
	if (!deathFlag_) {
		worldTransform_.translate = worldTransform_.translate + velocity_;

		walkTimer_ += 1.0f / 60.0f;

		float param = std::sin(2 * std::numbers::pi_v<float> *walkTimer_ / kWalkMotionTime);
		float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
		worldTransform_.rotate.x = Deg2Rad(radian);
	}
}

void Enemy::BehaviorDeathUpdate() {
	deathTimer_++;
	float x = deathTimer_ / deathTime_;
	x = 1.0f - std::powf((1.0f - x), 2.0f);
	worldTransform_.rotate.y = -std::numbers::pi_v<float> / 2.0f + (Deg2Rad(420) - Deg2Rad(90)) * x;
	worldTransform_.rotate.x = 0.0f + (Deg2Rad(420) - 0.0f) * x;
	if (deathTimer_ >= deathTime_) {
		deathFlag_ = true;
	}
}

void Enemy::BehaviorTransition() {
	if (behaviorRequest_ == BEHAVIOR::UnKnown) {
		return;
	}
	behavior_ = behaviorRequest_;
	switch (behavior_) {
	case BEHAVIOR::Walk:
		// 初期化
		break;
	case BEHAVIOR::Death:
		// 初期化
		deathTimer_ = 0.0f;
		break;
	}
	behaviorRequest_ = BEHAVIOR::UnKnown;
}
void Enemy::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (!deathFlag_) {
		if (model_) {
			Matrix4x4 world = Matrix4x4::Make::Affine(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

			Matrix4x4 uv = Matrix4x4::Make::Scale(uvTransform_.scale);
			uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
			uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate(uvTransform_.translate));

			model_->SetWVPData(camera_->DrawCamera(world), world, uv);
			model_->Draw(command, pso, light, tex);
		}
	}
}
void Enemy::OnCollision(const Player* player) {
	(void)player;
	if (behavior_ == BEHAVIOR::Death) {
		return;
	}
	if (player->IsAttack()) {
		behaviorRequest_ = BEHAVIOR::Death;
	}
	//deathFlag_ = true;
}

bool Enemy::IsDeathBehavior() const {
	if (behavior_ == BEHAVIOR::Death) {
		return true;
	}
	return false;
}