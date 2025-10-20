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

void WorldTransform::Set::Scale(const Vector3& scale) {
	parent_->transform_.scale_ = scale;
	if (parent_->isDirty_ == false) {
		parent_->isDirty_ = true;
	}
}

void WorldTransform::Set::Rotation(const Vector3& rotation) {
	parent_->transform_.rotation_ = rotation;
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
	isDirty_ = true;
}

void WorldTransform::LocalToWorld() {
	if (isDirty_ == false) {
		return;
	}
	mat_ = Matrix4x4::Make::Affine(get_.Scale(), get_.Rotation(), get_.Translation());
	isDirty_ = false;
}

const Vector3 WorldTransform::GetWorldPos()const {
	return { mat_.m[3][0] ,mat_.m[3][1] ,mat_.m[3][2] };
}

