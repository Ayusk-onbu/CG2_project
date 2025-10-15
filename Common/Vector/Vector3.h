#pragma once

struct Vector3 {
	float x, y, z;

	Vector3& operator+=(const Vector3& other);
	bool operator==(const Vector3& other)const;
	Vector3 operator-()const;
};

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);

Vector3 operator*(const Vector3& v1, const float& other);
Vector3 operator*(const float& other, const Vector3& v1);

Vector3 operator/(const Vector3& v1, const float& other);
Vector3 operator/(const float& other, const Vector3& v1);

//加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
//減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
//スカラー倍
Vector3 Multiply(float scaler, const Vector3& v);
Vector3 Multiply(const Vector3& v, const Vector3& v2);
//内積
float Dot(const Vector3& v1, const Vector3& v2);
//長さ
float Length(const Vector3& v);
//正規化
Vector3 Normalize(const Vector3& v);
//クロス積（外積）
Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);
//内積
float DotProduct(const Vector3& v1, const Vector3& v2);
//正射影
Vector3 Project(const Vector3& v1, const Vector3& v2);
//最近接点
//Vector3 ClosestPoint(const Vector3& point, const Segment& segment);
// 適当な垂直ベクトルを求める
Vector3 Perpendicular(const Vector3& vector);

Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);