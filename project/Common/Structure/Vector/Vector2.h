#pragma once

class Vector2 {
public:
	float x, y;
public:// Operator
	Vector2 operator-()const;
};

Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);

Vector2 operator*(const Vector2& v1, const float& other);
Vector2 operator*(const float& other, const Vector2& v1);
Vector2 operator*(const Vector2& v1, const Vector2& v2);

Vector2 operator/(const Vector2& v1, const float& other);
Vector2 operator/(const float& other, const Vector2& v1);