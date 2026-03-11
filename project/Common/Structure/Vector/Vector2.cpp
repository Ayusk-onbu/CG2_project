#include "Vector2.h"

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return{ v1.x + v2.x,v1.y + v2.y };
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return{ v1.x - v2.x,v1.y - v2.y };
}

Vector2 operator*(const Vector2& v1, const float& other) {
	return{ v1.x * other,v1.y * other };
}

Vector2 operator*(const float& other, const Vector2& v1) {
	return{ v1.x * other,v1.y * other };
}

Vector2 operator*(const Vector2& v1, const Vector2& v2) {
	return { v1.x * v2.x, v1.y * v2.y };
}

Vector2 operator/(const Vector2& v1, const float& other) {
	return{ v1.x / other,v1.y / other };
}
Vector2 operator/(const float& other, const Vector2& v1) {
	return{ v1.x / other,v1.y / other };
}