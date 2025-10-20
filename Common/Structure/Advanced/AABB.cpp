#include "AABB.h"

AABB AABB::World2AABB(Vector3 world) {
	Vector3 worldPos = world;
	AABB aabb;

	aabb.min = { worldPos.x - 0.5f, worldPos.y - 0.5f, worldPos.z - 0.5f };
	aabb.max = { worldPos.x + 0.5f, worldPos.y + 0.5f, worldPos.z + 0.5f };
	aabb.center();

	return aabb;
}

bool AABB::IsHitAABB2AABB(const AABB& a, const AABB& b) {
	if (a.min.x <= b.max.x && b.min.x <= a.max.x && a.min.y <= b.max.y && b.min.y <= a.max.y && a.min.z <= b.max.z && b.min.z <= a.max.z) {
		return true;
	}
	return false;
}

Vector3 AABB::center() const {
	Vector3 ret;
	ret.x = (min.x + max.x) / 2.0f;
	ret.y = (min.y + max.y) / 2.0f;
	ret.z = (min.z + max.z) / 2.0f;
	return ret;
}