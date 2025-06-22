#pragma once
#include "Vector3.h"

class Transform {
public:
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
	void Initialize() {
		scale = { 1.0f,1.0f,1.0f };
		rotate = { 0.0f,0.0f,0.0f };
		translate = { 0.0f,0.0f,0.0f };
	 }
};