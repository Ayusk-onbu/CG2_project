#pragma once
#include "Matrix4x4.h"
#include <vector>

Vector3 TransformNormal(Vector3& v, Matrix4x4& m);

float Lerp(float v1, float v2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);