#pragma once

struct Vector3 {
	float x, y, z;

	Vector3 operator+(const Vector3& other)const {
		return{ x + other.x,y + other.y, z + other.z };
	}
};