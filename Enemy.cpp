#include "Enemy.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "MathUtils.h"
#include "Player.h"
#include <algorithm>

Enemy::Enemy() {
	state_ = new EnemyStateApproach();
}
Enemy::~Enemy() {
	delete state_;
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void Enemy::Initialize(D3D12System& d3d12, ModelObject& model, CameraBase* camera, const Vector3& pos) {
	d3d12_ = &d3d12;
	model_ = model;
	model_.Initialize(d3d12, model.GetModelData());
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation({ 0.0f,Deg2Rad(180),0.0 });
}

void Enemy::GetBullet(ModelObject* model, Texture* texture) {
	bulletModel_ = model;
	bulletTex_ = texture;
}

void Enemy::Update() {

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	Vector3 pos = worldTransform_.get_.Translation();
	Vector3 rotation = worldTransform_.get_.Rotation();
	
	/*switch (phase_) {
	case Phase::Approach:
		ApproachUpdate(pos,rotation);
		break;
	case Phase::Leave:
		LeaveUpdate(pos,rotation);
		break;
	}*/
	//(this->*pFunc)(pos,rotation);
	//(this->*pFuncTable[static_cast<size_t>(phase_)])(pos, rotation);
	state_->UpDate(this, &pos, &rotation);
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->UpDate();
	}
	/*if (state_->IsShot()) {
		FireReset();
	}*/
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	ImGui::Begin("Player");

	ImGui::Text("SRT");
	if (ImGui::BeginTabBar("SRTBar")) {
		ImGuiManager::CreateImGui("Scale", worldTransform_.get_.Scale(), 0.0f, 10.0f);
		ImGuiManager::CreateImGui("Rotation", worldTransform_.get_.Rotation(), 0.0f, 180.0f);
		ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), 0.0f, 1280.0f);
		ImGui::EndTabBar();
	}

	ImGui::End();

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
}

void Enemy::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	model_.SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_, Matrix4x4::Make::Identity());
	model_.Draw(command, pso, light, tex);
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(*camera_, command, pso, light, *bulletTex_);
	}
}

void Enemy::ChangeState(EnemyState*state) {
	delete state_;
	state_ = state;
}

void Enemy::Fire() {
	/*if (--fireTimer_ <= 0.0f) {
		return;
	}*/
	Vector3 velocity;
	const float kBulletSpeed = 0.75f;
	Vector3 pos = {worldTransform_.mat_.m[3][0],worldTransform_.mat_.m[3][1], worldTransform_.mat_.m[3][2]};
	velocity = Normalize(player_->GetWorldPos() - pos) * kBulletSpeed;
	//velocity = TransformNormal(velocity, worldTransform_.mat_);

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(*d3d12_, bulletModel_, worldTransform_.get_.Translation(), velocity);
	bullets_.push_back(newBullet);
	//fireTimer_ = kFireTime_;
}

void Enemy::FireReset() {
	if (!state_->IsShot()) {
		return;
	}
	// 弾を発射する
	Fire();

	// 発射タイマーをセットする
	timedCalls_.push_back(
		new TimedCall(std::bind(&Enemy::FireReset, this), static_cast<uint32_t>(kFireTime_))
	);
}

void Enemy::SetTarget(const Player& player) {
	player_ = &player;
}

//void (Enemy::*Enemy::pFuncTable[])(Vector3& pos, Vector3& rotation) = {
//	&Enemy::ApproachUpdate,
//	&Enemy::LeaveUpdate
//};
//
//void Enemy::ApproachUpdate(Vector3& pos, Vector3& rotation) {
//	ApproachMove(pos);
//	ApproachRotate(rotation);
//}
//
//void Enemy::ApproachMove(Vector3& pos) {
//	pos.z -= kMoveSpeed_;
//	if (pos.z < 0.0f) {
//		//pFunc = &Enemy::LeaveUpdate;
//		phase_ = Phase::Leave;
//	}
//}
//
//void Enemy::ApproachRotate(Vector3& rotation) {
//	
//}
//
//void Enemy::LeaveUpdate(Vector3& pos, Vector3& rotation) {
//	LeaveMove(pos);
//	LeaveRotate(rotation);
//}
//
//void Enemy::LeaveMove(Vector3& pos) {
//	pos += kLeaveSpeed_;
//}
//
//void Enemy::LeaveRotate(Vector3& rotation) {
//	rotation.y += Deg2Rad(1);
//}