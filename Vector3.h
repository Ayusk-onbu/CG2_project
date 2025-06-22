#pragma once

struct Vector3 {
	float x, y, z;

	Vector3 operator+(const Vector3& other)const {
		return{ x + other.x,y + other.y, z + other.z };
	}
};


Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);