#pragma once
#include "Structures.h"

class Collider
{
public:
	virtual void OnCollision() { ; }
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

	uint32_t collisionAttribute_ = 0xffffffff;
	uint32_t collisionMask_ = 0xffffffff;
};

