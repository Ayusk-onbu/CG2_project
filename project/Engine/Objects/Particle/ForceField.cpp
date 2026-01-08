#include "ForceField.h"
#include <algorithm>
#include "CameraSystem.h"

// ------------------------------------
// Gravity Force Field 
// ------------------------------------

void GravityForceField::Initialize(Fngine* fngine) {
	obj_ = std::make_unique<ModelObject>();
	obj_->textureName_ = "GridLine";
	obj_->modelName_ = "debugBlock";
	obj_->Initialize(fngine);
}

void GravityForceField::ApplyForce(ParticleData* info) {
	float deltaTime = 1.0f / 60.0f;

	// 位置情報の取得
	Vector3 particlePos = info->worldTransform.transform_.translation_;
	Vector3 fieldCenter = worldTransform_.transform_.translation_;

	// Particleから場中心へのベクトル
	Vector3 direction = fieldCenter - particlePos;

	float distance = Length(direction);

	if (distance <= 0.0001f || distance > radius_) {
		return; // 影響範囲外
	}

	info->velocity.x += direction_.x * deltaTime;
	info->velocity.y += direction_.y * deltaTime;
	info->velocity.z += direction_.z * deltaTime;
}

void GravityForceField::DrawDebug() {
	Vector3 pos = worldTransform_.get_.Translation();
	std::string name = "GravityForce" + name_;

	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat3("Direction", &direction_.x);
	ImGui::DragFloat("radius", &radius_);
	if (ImGui::Button("IsView")) {
		isVisible_ = isVisible_ == true ? false : true;
	}
	ImGui::End();

	worldTransform_.set_.Translation(pos);

	if (isVisible_) {
		// 方向と大きさが分かるようにする
		obj_->worldTransform_.set_.Translation(worldTransform_.get_.Translation());
		obj_->worldTransform_.set_.Scale({ radius_ * 2.0f,radius_ * 2.0f ,radius_ * 2.0f });
		obj_->LocalToWorld();
		obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
		obj_->Draw(ObjectDrawType::WIREFRAME);
	}
}

// ------------------------------------
// Point Force Field
// ------------------------------------

void PointForceField::Initialize(Fngine* fngine) {
	obj_ = std::make_unique<ModelObject>();
	obj_->modelName_ = "bullet";
	obj_->textureName_ = "GridLine";
	obj_->Initialize(fngine);
}

void PointForceField::ApplyForce(ParticleData* info) {
	// 位置情報の取得
	Vector3 particlePos = info->worldTransform.get_.Translation();
	Vector3 fieldCenter = worldTransform_.get_.Translation();

	// Particleから場中心へのベクトル
	Vector3 direction = fieldCenter - particlePos;

	float distance = Length(direction);

	if (distance <= 0.0001f || distance > radius_) {
		return; // 影響範囲外
	}

	// 正規化
	Vector3 normalizedDirection = direction / distance;

	// フォールオフ（距離で力を減衰）
	float falloff = std::pow(std::clamp((radius_ - distance) / radius_, 0.0f, 1.0f), 2.0f);

	float forceMagnitude = strength_ * falloff;

	// 加算
	info->velocity += normalizedDirection * forceMagnitude;
}

void PointForceField::DrawDebug() {

	Vector3 pos = worldTransform_.get_.Translation();
	std::string name = "PointForce" + name_;

	ImGui::Begin(name.c_str());
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat("strength", &strength_);
	ImGui::DragFloat("radius", &radius_);
	if (ImGui::Button("IsView")) {
		isVisible_ = isVisible_ == true ? false : true;
	}
	ImGui::End();

	worldTransform_.set_.Translation(pos);

	if (isVisible_) {
		// 吸引は青、斥力は赤
		Vector4 color = (strength_ > 0) ? Vector4{ 0.0f,0.2f,1.0f,1.0f } : Vector4{ 1.0f,0.2f,0.0f,1.0f };
		// 方向と大きさが分かるようにする
		obj_->SetColor(color);
		obj_->worldTransform_.set_.Translation(worldTransform_.get_.Translation());
		obj_->worldTransform_.set_.Scale({ radius_,radius_ ,radius_ });
		obj_->LocalToWorld();
		obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
		obj_->Draw(ObjectDrawType::WIREFRAME);
	}
}