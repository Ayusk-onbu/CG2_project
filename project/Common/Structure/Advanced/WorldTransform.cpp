#include "WorldTransform.h"

Vector3& WorldTransform::Get::Scale() {
	return parent_->transform_.scale_;
}

Vector3& WorldTransform::Get::Rotation() {
	return parent_->transform_.rotation_;
}

Vector3& WorldTransform::Get::Translation() {
	return parent_->transform_.translation_;
}

Quaternion& WorldTransform::Get::Quaternion() {
	return parent_->quaternion_;
}

Vector3 WorldTransform::Get::ForwardVector() {
	// この世界での前方を定義
	Vector3 forward = { 0.0f,0.0f,1.0f };
	auto currentQ = parent_->get_.Quaternion();

	Vector3 worldForward = Quaternion::RotateVector(forward, currentQ);
	return worldForward;
}

void WorldTransform::Set::Scale(const Vector3& scale) {
	parent_->transform_.scale_ = scale;
	if (parent_->isDirty_ == false) {
		parent_->isDirty_ = true;
	}
}

void WorldTransform::Set::Rotation(const Vector3& rotation) {
	parent_->transform_.rotation_ = rotation;
	parent_->quaternion_ = Quaternion::MakeRotateXYZ(rotation);
	if (parent_->isDirty_ == false) {
		parent_->isDirty_ = true;
	}
}

void WorldTransform::Set::Translation(const Vector3& translation) {
	parent_->transform_.translation_ = translation;
	if (parent_->isDirty_ == false) {
		parent_->isDirty_ = true;
	}
}

void WorldTransform::Initialize() {
	transform_.Initialize();
	quaternion_.Initialize();
	isDirty_ = true;
}

void WorldTransform::LocalToWorld() {
	if (isDirty_ == false) {
		return;
	}
	
	Matrix4x4 scaleMat = Matrix4x4::Make::Scale(get_.Scale());
	Matrix4x4 rotationMat = Quaternion::MakeRotateMatrix(quaternion_);
	Matrix4x4 translateMat = Matrix4x4::Make::Translate(get_.Translation());

	// S * R * T
	mat_ = Matrix4x4::Multiply(scaleMat, rotationMat);
	mat_ = Matrix4x4::Multiply(mat_, translateMat);
	//mat_ = Matrix4x4::Make::Affine(get_.Scale(), get_.Rotation(), get_.Translation());

	isDirty_ = false;
}

const Vector3 WorldTransform::GetWorldPos()const {
	return { mat_.m[3][0] ,mat_.m[3][1] ,mat_.m[3][2] };
}

void WorldTransform::LookAtToVector(const Vector3& target) {

	Vector3 globalUp = { 0.0f,1.0f,0.0f };

	// 前方ベクトルを計算
	Vector3 forward = target - GetWorldPos();

	Vector3 newForward = Normalize(forward);

	// 右ベクトルを計算
	Vector3 newRight = CrossProduct(globalUp, newRight);

	newRight = Normalize(newRight);

	Vector3 newUp = CrossProduct(newForward, newRight);

	Quaternion q = Quaternion::MakeFromBasis(newRight,newUp,newForward);

	set_.Quaternion(Quaternion::Slerp(get_.Quaternion(),q,0.1f));
}

