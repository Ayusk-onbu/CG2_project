#include "Player.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "MathUtils.h"
#include <algorithm>

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Initialize(D3D12System& d3d12, std::unique_ptr<ModelObject>model,CameraBase* camera) {
	d3d12_ = &d3d12;
	model_ = move(model);// 所有権の以降
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::GetBullet(ModelObject* model, Texture* texture) {
	bulletModel_ = model;
	bulletTex_ = texture;
}

void Player::Update() {

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	Vector3 pos = worldTransform_.get_.Translation();
	Vector3 rotation = {worldTransform_.get_.Rotation().x,worldTransform_.get_.Rotation().y,0.0f};
	//worldTransform_.set_.Rotation({0.0f,worldTransform_.get_.Rotation().y + 0.01f,0.0f});
	
	Move(pos);
	Rotate(rotation);
	Attack();

	for (PlayerBullet* bullet:bullets_) {
		if (bullet) {
			bullet->Update();
		}
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

	pos.x = std::clamp(pos.x, -kMoveLimitX_.x, kMoveLimitX_.x);
	pos.y = std::clamp(pos.y, -kMoveLimitX_.y, kMoveLimitX_.y);

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_,Matrix4x4::Make::Identity());
	model_->Draw(command,pso,light,tex);
	for (PlayerBullet* bullet : bullets_) {
		if (bullet) {
			bullet->Draw(*camera_, command, pso, light, *bulletTex_);
		}
	}
}

const Vector3 Player::GetWorldPos()const {
	return { worldTransform_.mat_.m[3][0] ,worldTransform_.mat_.m[3][1] ,worldTransform_.mat_.m[3][2] };
}

void Player::Move(Vector3& pos) {
	if (InputManager::GetKey().PressKey(DIK_LEFT)) {
		pos.x -= kMoveSpeed_;
	}
	else if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
		pos.x += kMoveSpeed_;
	}
	if (InputManager::GetKey().PressKey(DIK_DOWN)) {
		pos.y -= kMoveSpeed_;
	}
	else if (InputManager::GetKey().PressKey(DIK_UP)) {
		pos.y += kMoveSpeed_;
	}
}

void Player::Rotate(Vector3& rotation) {
	if (InputManager::GetKey().PressKey(DIK_LEFT)) {
		rotation.z = -75.0f;
	}
	else if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
		rotation.z = 75.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_A)) {
		rotation.y -= Deg2Rad(kRotSpeed_);
	}
	else if (InputManager::GetKey().PressKey(DIK_D)) {
		rotation.y += Deg2Rad(kRotSpeed_);
	}
}

void Player::Attack() {
	if (InputManager::GetKey().PressedKey(DIK_SPACE)) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0.0f, 0.0f, kBulletSpeed);

		velocity = TransformNormal(velocity, worldTransform_.mat_);

		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(*d3d12_, bulletModel_, worldTransform_.get_.Translation(),velocity);
		bullets_.push_back(newBullet);
	}
}
