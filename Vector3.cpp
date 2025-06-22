#include "Vector3.h"


Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t) {
	Vector3 result = {};
	result.x = p0.x * (1 - t) + p1.x * t;
	result.y = p0.y * (1 - t) + p1.y * t;
	result.z = p0.z;
	return result;
}
