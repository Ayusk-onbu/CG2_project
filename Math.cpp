#include "Math.h"
#include <cassert>

Vector3 TransformV(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 ret;
	ret.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0]
		+ vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	ret.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1]
		+ vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	ret.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2]
		+ vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float W = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3]
		+ vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(W != 0.0f);
	ret.x /= W;
	ret.y /= W;
	ret.z /= W;
	return ret;
}