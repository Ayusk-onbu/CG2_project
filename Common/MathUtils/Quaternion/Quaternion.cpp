#include "Quaternion.h"
#include <cmath>

Quaternion Quaternion::Multiply(const Quaternion& q, const Quaternion& r) {
	Quaternion result;
	result.w = q.w * r.w - q.x * r.x - q.y * r.y - q.z * r.z;
	result.x = q.w * r.x + q.x * r.w + q.y * r.z - q.z * r.y;
	result.y = q.w * r.y - q.x * r.z + q.y * r.w + q.z * r.x;
	result.z = q.w * r.z + q.x * r.y - q.y * r.x + q.z * r.w;
	return result;
}

Quaternion Quaternion::Identity() {
	Quaternion result;
	result.w = 1.0f;
	result.x = 0.0f;
	result.y = 0.0f;
	result.z = 0.0f;
	return result;
}

Quaternion Quaternion::Conjugate(const Quaternion& q) {
	Quaternion result;
	result.w = q.w;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	return result;
}

float Quaternion::Norm(const Quaternion& q) {
	return std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

Quaternion Quaternion::Normalize(const Quaternion& q) {
	Quaternion result;
	float norm = Norm(q);
	if (norm > 0.0f) {
		result.w = q.w / norm;
		result.x = q.x / norm;
		result.y = q.y / norm;
		result.z = q.z / norm;
	}
	else {
		result = Identity(); // ノルムが0の場合は単位クォータニオンを返す
	}
	return result;
}

Quaternion Quaternion::Inverse(const Quaternion& q) {
	Quaternion result;
	float normSq = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
	if (normSq > 0.0f) {
		Quaternion conj = Conjugate(q);
		result.w = conj.w / normSq;
		result.x = conj.x / normSq;
		result.y = conj.y / normSq;
		result.z = conj.z / normSq;
	}
	else {
		result = Identity(); // ノルムが0の場合は単位クォータニオンを返す
	}
	return result;
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion result;
	float halfAngle = angle * 0.5f;
	float s = std::sin(halfAngle);
	result.w = std::cos(halfAngle);
	result.x = axis.x * s;
	result.y = axis.y * s;
	result.z = axis.z * s;
	return Quaternion::Normalize(result);
}


Vector3 Quaternion::RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion p = { v.x, v.y, v.z, 0.0f };
	Quaternion qConj = Conjugate(q);
	Quaternion resultQuat = Multiply(Multiply(q, p), qConj);
	return { resultQuat.x, resultQuat.y, resultQuat.z };
}


Matrix4x4 Quaternion::MakeRotateMatrix(const Quaternion& q) {
	Matrix4x4 result = {};
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float ww = q.w * q.w;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	result.m[0][0] = ww + xx - yy - zz;
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = ww - xx + yy - zz;
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = ww - xx - yy + zz;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float t) {

	Quaternion normQ0 = Normalize(q0);
	Quaternion normQ1 = Normalize(q1);

	// 二つのQuaternionの間の角度を求める
	// dot = cosθ
	float dot = normQ0.x * normQ1.x + normQ0.y * normQ1.y + normQ0.z * normQ1.z + normQ0.w * normQ1.w;
	if (dot < 0) {
		normQ1.x = -normQ1.x;
		normQ1.y = -normQ1.y;
		normQ1.z = -normQ1.z;
		normQ1.w = -normQ1.w;
		dot = -dot;
	}

	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float sinOne_theta = std::sin((1.0f - t) * theta) / sinTheta;
	float sinT_theta = std::sin(t * theta) / sinTheta;

	return {
		normQ0.x * sinOne_theta + normQ1.x * sinT_theta,
		normQ0.y * sinOne_theta + normQ1.y * sinT_theta,
		normQ0.z * sinOne_theta + normQ1.z * sinT_theta,
		normQ0.w * sinOne_theta + normQ1.w * sinT_theta
	};
}

//=================================
// Operator Overload
//=================================

Quaternion operator*(const Quaternion& q,const Quaternion& r) {
	return Quaternion::Multiply(q, r);
}

Quaternion& Quaternion::operator*=(const Quaternion& r) {
	*this = Multiply(*this, r);
	return *this;
}