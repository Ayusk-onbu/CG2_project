#include "DynamicObject.h"

void DynamicObject::Initialize(Fngine* engine, std::string modelName, std::string textureName) {
	transform_.Initialize();
	transform_.LocalToWorld();
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