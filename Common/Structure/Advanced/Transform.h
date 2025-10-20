#pragma once
#include "Vector3.h"

class Transform {
public:
	Vector3 scale_;
	Vector3 rotation_;
	Vector3 translation_;
public:
	void Initialize();
};