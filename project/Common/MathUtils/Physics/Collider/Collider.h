#pragma once
#include "Structures.h"
#include "AABB.h"

enum COLLISIONATTRIBUTE :int{
	COL_None = 0,
	COL_Player = 1 << 0,
	COL_Enemy = 1 << 1,
	COL_Player_Attack = 1 << 2,
	COL_Enemy_Attack = 1 << 3,
};

class Collider
{
public:
	virtual void OnCollision(Collider* other) { ; }
	virtual const Vector3 GetWorldPosition() = 0;
	const float& GetRadius()const { return radius_; }
	void SetRadius(const float& radius) { radius_ = radius; }
public:
	const uint32_t& GetMyType()const { return collisionAttribute_; }
	const uint32_t& GetYourType()const { return collisionMask_; }
	void SetMyType(const uint32_t& type) { collisionAttribute_ = type; }
	void SetYourType(const uint32_t& type) { collisionMask_ = type; }
private:
	float radius_ = 1.0f;
	AABB aabb_;

	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;
};

class AttackCollider
	:public Collider
{
public:
	const Vector3 GetWorldPosition()override { return worldPosition_; }
	void SetWorldPosition(const Vector3& pos) { worldPosition_ = pos; }
private:
	Vector3 worldPosition_ = { 0.0f,0.0f,0.0f };
};

