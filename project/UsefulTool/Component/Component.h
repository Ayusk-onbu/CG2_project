#pragma once
#include "Structures.h"

class DynamicObject;

class Component {
public:
	DynamicObject* master_ = nullptr;

	virtual ~Component() = default;
	virtual void Initialize() {}
	virtual void Update(float deltaTime){}
	virtual void Draw() {}
	// 衝突が起きたときに呼ぶ関数
	virtual void OnCollision(DynamicObject* other, const Vector3& pushOut) {}
};