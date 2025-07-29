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
	worldTransform_.set_.Translation({ 0.0f, 0.0f, camera_->GetRadius()});
	worldTransform3DReticle_.Initialize();
	worldTransform3DReticle_.set_.Rotation({ 0.0f, Deg2Rad(180), 0.0f });
	SetMyType(0b1);
	SetYourType(~0b1);
}

void Player::SetBullet(ModelObject* model, Texture* texture) {
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
	if (InputManager::GetKey().PressKey(DIK_RETURN)) {
		GetCursor();
	}
	else {
		ReticleUpdate();
	}
	Attack();


	for (PlayerBullet* bullet:bullets_) {
		if (bullet) {
			bullet->Update();
		}
	}
#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::Text("SRT");
	if (ImGui::BeginTabBar("SRTBar")) { 
		ImGuiManager::CreateImGui("Scale", worldTransform_.get_.Scale(), 0.0f, 10.0f);
		ImGuiManager::CreateImGui("Rotation", worldTransform_.get_.Rotation(), 0.0f, 180.0f);
		ImGuiManager::CreateImGui("Translation", worldTransform_.get_.Translation(), 0.0f, 1280.0f);
		ImGui::EndTabBar();
	}
	
	ImGui::End();
#endif // DEBUG

	pos.x = std::clamp(pos.x, -kMoveLimitX_.x, kMoveLimitX_.x);
	pos.y = std::clamp(pos.y, -kMoveLimitX_.y, kMoveLimitX_.y);

	worldTransform_.set_.Translation(pos);
	worldTransform_.set_.Rotation(rotation);
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	worldTransform_.LocalToWorld();
	worldTransform_.mat_ = Matrix4x4::Multiply(worldTransform_.mat_, *parentMat_);
	model_->SetWVPData(camera_->DrawCamera(worldTransform_.mat_), worldTransform_.mat_,Matrix4x4::Make::Identity());
	model_->Draw(command,pso,light,tex);
	for (PlayerBullet* bullet : bullets_) {
		if (bullet) {
			bullet->Draw(*camera_, command, pso, light, *bulletTex_);
		}
	}
	//Matrix4x4 drawSpriteMat = camera_->DrawCamera(worldTransform3DReticle_.mat_);
	//Vector4 worldPos = { drawSpriteMat.m[3][0],drawSpriteMat.m[3][1] ,drawSpriteMat.m[3][2],1.0f };
	//Vector4 clip = Matrix4x4::Transform(camera_->GetViewProjectionMatrix(),worldPos);
	//clip.x /= clip.w;
	//clip.y /= clip.w;
	//clip.z /= clip.w;
	//float screenX = (clip.x * 0.5f + 0.0f) * 128.0f;
	//float screenY = (clip.y * 0.5f + 0.0f) * 72.0f;
	//drawSpriteMat.m[3][0] = screenX;
	//drawSpriteMat.m[3][1] = screenY;
	//drawSpriteMat.m[3][2] = clip.z;

	sprite2DReticle_->SetWVPData(camera_->DrawCamera(worldTransform3DReticle_.mat_), worldTransform3DReticle_.mat_, Matrix4x4::Make::Identity());
	//sprite2DReticle_->SetWVPData(drawSpriteMat, worldTransform3DReticle_.mat_, Matrix4x4::Make::Identity());
	sprite2DReticle_->Draw(command, pso, light, *sprite2DReticleTex_);
}

void Player::OnCollision() {

}

//const Vector3 Player::GetWorldPos()const {
//	return { worldTransform_.mat_.m[3][0] ,worldTransform_.mat_.m[3][1] ,worldTransform_.mat_.m[3][2] };
//}

void Player::Move(Vector3& pos) {
	SHORT lx = InputManager::GetGamePad(0).GetLeftStickX();
	SHORT ly = InputManager::GetGamePad(0).GetLeftStickY();

	pos.x += static_cast<float>(lx) / 32767.0f * kMoveSpeed_;
	pos.y += static_cast<float>(ly) / 32767.0f * kMoveSpeed_;

	if (InputManager::GetMouse().IsButtonRelease(1)) {
		if (pos.z >= 1.0f) {
			pos.z = 0.0f;
		}
		else {
			pos.z = 50.0f;
		}
	}

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
	/*if (!InputManager::GetGamePad(0).IsConnected()) {
		return;
	}*/
	// Rボタンで攻撃
	if (InputManager::GetGamePad(0).IsPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0.0f, 0.0f, kBulletSpeed);

		//velocity = TransformNormal(velocity, worldTransform_.mat_);
		velocity = worldTransform3DReticle_.GetWorldPos() - worldTransform_.GetWorldPos();
		velocity = Normalize(velocity) * kBulletSpeed;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(*d3d12_, bulletModel_, { worldTransform_.GetWorldPos().x,worldTransform_.GetWorldPos().y + 0.5f,worldTransform_.GetWorldPos().z }, velocity);
		bullets_.push_back(newBullet);
	}

	if (InputManager::GetKey().PressedKey(DIK_SPACE)) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0.0f, 0.0f, kBulletSpeed);

		//velocity = TransformNormal(velocity, worldTransform_.mat_);
		velocity = worldTransform3DReticle_.GetWorldPos() - worldTransform_.GetWorldPos();
		velocity = Normalize(velocity) * kBulletSpeed;
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(*d3d12_, bulletModel_, { worldTransform_.GetWorldPos().x,worldTransform_.GetWorldPos().y,worldTransform_.GetWorldPos().z }, velocity);
		bullets_.push_back(newBullet);
	}
}

void Player::ReticleUpdate() {
	//// 自機から3Dレティクルへの距離
	//const float kDistancePlayerTo3DReticle = 50.0f;
	//// 自機から3Dレティクルへのオフセット(Z+向き)
	//Vector3 offset = { 0.0f,0.0f,1.0f };
	//// 自機からワールド行列の回転を反映
	////offset = Matrix4x4::Transform(offset,worldTransform_.mat_);
	//// ベクトルの長さを整える
	//offset = Normalize(offset) * kDistancePlayerTo3DReticle;
	//// 3Dレティクルの座標を設定
	//worldTransform3DReticle_.set_.Translation(worldTransform_.GetWorldPos() + offset);
	//worldTransform3DReticle_.LocalToWorld();
	
	Vector3 reticlePos = worldTransform3DReticle_.GetWorldPos();
	SHORT lx = InputManager::GetGamePad(0).GetRightStickX();
	SHORT ly = InputManager::GetGamePad(0).GetRightStickY();

	reticlePos.x += static_cast<float>(lx) / 32767.0f * kMoveSpeed_  *2.5f;
	reticlePos.y += static_cast<float>(ly) / 32767.0f * kMoveSpeed_  *2.5f;
	reticlePos.z = worldTransform_.GetWorldPos().z;


	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset = { 0.0f,0.0f,1.0f };
	// 自機からワールド行列の回転を反映
	//offset = Matrix4x4::Transform(offset,worldTransform_.mat_);
	// ベクトルの長さを整える
	offset = Normalize(offset) * kDistancePlayerTo3DReticle;
	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.set_.Translation(reticlePos + offset);
	worldTransform3DReticle_.LocalToWorld();
}

void Player::GetCursor() {
	// マウスの座標を取得
	InputManager::GetMouse().GetPosition(pos2DReticle_);

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV = camera_->GetViewProjectionMatrix();
	// 合成行列の逆行列を計算
	Matrix4x4 matInverseVPV = Matrix4x4::Inverse(matVPV);

	pos2DReticle_.x *= 2.0f;
	pos2DReticle_.y *= 2.0f; // Y座標を反転

	// スクリーン座標
	Vector4 posNear = Vector4(pos2DReticle_.x,pos2DReticle_.y,0.0f,1.0f);
	posNear = Matrix4x4::Transform(matInverseVPV,posNear);
	posNear.x /= posNear.w;
	posNear.y /= posNear.w;
	posNear.z /= posNear.w;
	Vector4 posFar = Vector4(pos2DReticle_.x, pos2DReticle_.y,1.0f, 1.0f);
	posFar = Matrix4x4::Transform(matInverseVPV,posFar);
	posFar.x /= posFar.w;
	posFar.y /= posFar.w;
	posFar.z /= posFar.w;

	// マウスレイの方向
	Vector3 mouseDirection = Vector3(posFar.x - posNear.x, posFar.y - posNear.y, posFar.z - posNear.z);
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	worldTransform3DReticle_.set_.Translation(/*worldTransform_.GetWorldPos() + */Vector3(posNear.x, posNear.y, posNear.z) + mouseDirection * kDistanceTestObject);
	worldTransform3DReticle_.LocalToWorld();
	//worldTransform3DReticle_.mat_ = Matrix4x4::Multiply(worldTransform3DReticle_.mat_,worldTransform_.mat_);

#ifdef _DEBUG
	ImGui::Begin("Reticle");
	ImGuiManager::CreateImGui("mouse Position", pos2DReticle_, 0.0f, 1280.0f);
	ImGuiManager::CreateImGui("3D Reticle Position", worldTransform3DReticle_.get_.Translation(), -50.0f, 50.0f);
	ImGuiManager::CreateImGui("Mouse Direction", mouseDirection, -1.0f, 1.0f);
	ImGuiManager::CreateImGui("Mouse Near Position", posNear, -50.0f, 50.0f);
	ImGuiManager::CreateImGui("Mouse Far Position", posFar, -50.0f, 50.0f);
	ImGui::End();
#endif // _DEBUG
}