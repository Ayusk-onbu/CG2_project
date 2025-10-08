#pragma once
#include "Structures.h"

class Quaternion final
{
public:
	float x, y, z, w;
public:
	/// <summary>
	/// Quaternion同士の積
	/// </summary>
	/// <param name="q"></param>
	/// <param name="r"></param>
	/// <returns></returns>
	static Quaternion Multiply(const Quaternion& q, const Quaternion& r);

	/// <summary>
	/// 単位クォータニオンを返す
	/// </summary>
	/// <returns></returns>
	static Quaternion Identity();

	/// <summary>
	/// 共役クォータニオンを返す
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	static Quaternion Conjugate(const Quaternion& q);

	/// <summary>
	/// クォータニオンのノルムを返す
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	static float Norm(const Quaternion& q);

	/// <summary>
	/// クォータニオンの正規化を返す
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	static Quaternion Normalize(const Quaternion& q);

	/// <summary>
	/// クォータニオンの逆元を返す
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	static Quaternion Inverse(const Quaternion& q);

	/// <summary>
	/// 回転軸と回転角からクォータニオンを生成
	/// </summary>
	/// <param name="axis 回転軸"></param>
	/// <param name="angle 回転角"></param>
	/// <returns></returns>
	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	/// <summary>
	/// ベクトルを回転させる
	/// </summary>
	/// <param name="v"></param>
	/// <param name="q"></param>
	/// <returns></returns>
	static Vector3 RotateVector(const Vector3& v, const Quaternion& q);

	/// <summary>
	/// クォータニオンから回転行列を生成
	/// </summary>
	/// <param name="q"></param>
	/// <returns></returns>
	static Matrix4x4 MakeRotateMatrix(const Quaternion& q);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="q0"></param>
	/// <param name="q1"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
public:
	Quaternion& operator*=(const Quaternion& r);
};

Quaternion operator*(const Quaternion& q, const Quaternion& r);
