#include "DynamicObject.h"

void DynamicObject::Initialize(Fngine* engine, std::string modelName, std::string textureName) {
	
}

void DynamicObject::Update(float deltaTime) {
	for (auto& comp : components_) {
		comp->Update(deltaTime);
	}
}

void DynamicObject::Draw() {

}

void DynamicObject::OnCollision(DynamicObject* other, const Vector3& pushOut) {
	// 自分が持っているすべてのコンポーネントに「当たったぞ！」と通知する
	for (auto& comp : components_) {
		comp->OnCollision(other, pushOut);
	}
}

//
//void DynamicObject::OnCollisionGround(Collider* other, const Vector3& outPush) {
//	Vector3 actualPush = -outPush;
//	Vector3 normal = actualPush;
//	float len2{ Dot(actualPush, actualPush) };
//	if (len2 > 0.0f) {
//		normal = normal / sqrt(len2);
//	}
//
//	if(other->GetMyType() == COL_Static_Map) {
//		currentGroundFriction_ = 0.8f;
//	}
//	else if (other->GetMyType() == COL_Static_Map) {
//		currentGroundFriction_ = 0.2f;
//	}
//	else {
//		// 他のものなら何もしない
//		return;
//	}
//	
//	obj_->worldTransform_.set_.Translation(obj_->worldTransform_.get_.Translation() + actualPush);
//
//	// 足元に地面があるかのチェック
//	if (normal.y > 0.8f) {
//		onGround_ = true;
//	}
//	else {
//		// 床として判定できない場合は摩擦、フラグ等をリセット
//		currentGroundFriction_ = 0.0f;
//	}
//}