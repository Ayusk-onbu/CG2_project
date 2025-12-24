#pragma once
#include "Structures.h"
#include <vector>
#include "Quaternion.h"
#include "Keyframe.h"

Vector3 TransformNormal(Vector3& v, Matrix4x4& m);

float Lerp(float v1, float v2, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

#pragma region Distance
float Distance(const Vector2& a, const Vector2& b);
#pragma endregion

/// <summary>
	/// 指定時刻の値を取得する
	/// </summary>
	/// <param name="keyframes"></param>
	/// <param name="time">指定する時間</param>
	/// <returns></returns>
Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

/// <summary>
/// 指定時刻の値を取得する
/// </summary>
/// <param name="keyframes"></param>
/// <param name="time">指定する時間</param>
/// <returns></returns>
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);