#include "Structures.h"

class AABB {
public:
	static AABB World2AABB(Vector3 world);
	static bool IsHitAABB2AABB(const AABB& a, const AABB& b);

public:
	Vector3 min;
	Vector3 max;
	Vector3 center()const;
};