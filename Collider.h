#pragma once
#include "Structures.h"

class Collider
{
public:
	virtual void OnCollision() { ; }
	virtual const Vector3 GetWorldPosition() = 0;
	const float& GetRadius()const{ return radius_; }
	void SetRadius(const float& radius) { radius_ = radius; }
private:
	float radius_ = 0.5f;
};

