#include "MathUtils.h"
#include <algorithm>
#include <cmath>

Vector3 TransformNormal(Vector3& v, Matrix4x4& m) {
	Vector3 ret{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};
	return ret;
}

float Lerp(float v1, float v2, float t) {
	return v1 + (v2 - v1) * t;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return {
		Lerp(v1.x,v2.x,t),
		Lerp(v1.y,v2.y,t),
		Lerp(v1.z,v2.z,t)
	};
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	Vector3 v1N = Normalize(v1);
	Vector3 v2N = Normalize(v2);

	float dot = Dot(v1N, v2N);

	dot = std::clamp(dot, -0.9999f, 1.0f);

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1.0f - t) * theta);
	float sinThetaTo = std::sin(t * theta);
	Vector3 normaLerpV;
	if (sinTheta < 1.0e-5) {
		normaLerpV = v1N;
	}
	else {
		normaLerpV = (sinThetaFrom * v1N + sinThetaTo * v2N) / sinTheta;
	}
	
	float length1 = Length(v1);
	float length2 = Length(v2);
	
	float length = Lerp(length1, length2, t);

	return length * normaLerpV;
}