#include "Vector3.h"
#include <cmath>

Vector3& Vector3::operator+=(const Vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

bool Vector3::operator==(const Vector3& other)const {
	return (x == other.x && y == other.y && z == other.z);
}

Vector3 Vector3::operator-() const{
	return Vector3(-x, -y, -z);
}

Vector3 operator+(const Vector3& v1, const Vector3& v2){
	return{ v1.x + v2.x,v1.y + v2.y, v1.z + v2.z };
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return{ v1.x - v2.x,v1.y - v2.y, v1.z - v2.z };
}

Vector3 operator*(const Vector3& v1, const float& other) {
	return{ v1.x * other,v1.y * other, v1.z * other };
}

Vector3 operator*(const float& other, const Vector3& v1) {
	return{ v1.x * other,v1.y * other, v1.z * other };
}

Vector3 operator/(const Vector3& v1, const float& other) {
	return{ v1.x / other,v1.y / other, v1.z / other };
}
Vector3 operator/(const float& other, const Vector3& v1) {
	return{ v1.x / other,v1.y / other, v1.z / other };
}

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}
Vector3 Multiply(const Vector3& v, const Vector3& v2) {
	Vector3 ret;
	ret.x = v.x * v2.x;
	ret.y = v.y * v2.y;
	ret.z = v.z * v2.z;
	return ret;
}
//スカラー倍
Vector3 Multiply(float scaler, const Vector3& v) {
	Vector3 ret;
	ret.x = v.x * scaler;
	ret.y = v.y * scaler;
	ret.z = v.z * scaler;
	return ret;
}
//内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float ret;
	ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}
//長さ
float Length(const Vector3& v) {
	float ret;
	ret = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return ret;
}
//正規化
Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	if (length == 0.0f) {
		return v;
	}
	Vector3 ret;
	ret.x = v.x / length;
	ret.y = v.y / length;
	ret.z = v.z / length;
	return ret;
}

//クロス積（外積）
Vector3 CrossProduct(const Vector3& v1, const Vector3& v2) {
	Vector3 ret = {};
	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;
	return ret;
}

//内積
float DotProduct(const Vector3& v1, const Vector3& v2) {
	float ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return ret;
}

//正射影
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 ret;
	float dot = Dot(v1, Normalize(v2));
	ret = Multiply(dot, Normalize(v2));
	return ret;
}



//最近接点
//Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
//	// 線分の始点と終点を計算
//	Vector3 segmentEnd = { segment.origin.x + segment.diff.x,
//						   segment.origin.y + segment.diff.y,
//						   segment.origin.z + segment.diff.z };
//
//	// 点から線分の始点へのベクトル
//	Vector3 pointVec = { point.x - segment.origin.x,
//						 point.y - segment.origin.y,
//						 point.z - segment.origin.z };
//
//	// `Project` を使用して正射影を求める
//	Vector3 projectedVec = Project(pointVec, segment.diff);
//
//	// 線分の始点からの正射影点を計算
//	Vector3 projectedPoint = { segment.origin.x + projectedVec.x,
//							   segment.origin.y + projectedVec.y,
//							   segment.origin.z + projectedVec.z };
//	return projectedPoint;
//
//}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		// XまたはYが０なら
		// v1x * v2x + v1y * v2y + v1z * v2z
		// x or y の片方が消える
		// 任意の点も0を持てば良い話
		return { -vector.y,vector.x,0.0f };
	}
	// Xを消して同じ値で相殺
	return{ 0.0f,-vector.z,vector.y };
}