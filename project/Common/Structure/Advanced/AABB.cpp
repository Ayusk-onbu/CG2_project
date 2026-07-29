#include "AABB.h"

#include <algorithm>
#include <cfloat>

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

static AABB TransformAABB(const AABB& aabb, const Matrix4x4& mat) {
	// 元の AABB の 8 つの頂点を抽出
	Vector3 corners[8] = {
		{ aabb.min.x, aabb.min.y, aabb.min.z },
		{ aabb.max.x, aabb.min.y, aabb.min.z },
		{ aabb.min.x, aabb.max.y, aabb.min.z },
		{ aabb.max.x, aabb.max.y, aabb.min.z },
		{ aabb.min.x, aabb.min.y, aabb.max.z },
		{ aabb.max.x, aabb.min.y, aabb.max.z },
		{ aabb.min.x, aabb.max.y, aabb.max.z },
		{ aabb.max.x, aabb.max.y, aabb.max.z }
	};

	// 0番目の頂点を変換し、新しい min / max の基準にする
	Vector4 firstTransformed = Matrix4x4::Transform(mat, Vector4{ corners[0].x, corners[0].y, corners[0].z, 1.0f });
	AABB newAABB;
	newAABB.min = { firstTransformed.x, firstTransformed.y, firstTransformed.z };
	newAABB.max = newAABB.min;

	// 残り 7 つの頂点を変換して、新しい min / max を外側に広げていく
	for (int i = 1; i < 8; ++i) {
		Vector4 transformed = Matrix4x4::Transform(mat, Vector4{ corners[i].x, corners[i].y, corners[i].z, 1.0f });

		newAABB.min.x = (std::min)(newAABB.min.x, transformed.x);
		newAABB.min.y = (std::min)(newAABB.min.y, transformed.y);
		newAABB.min.z = (std::min)(newAABB.min.z, transformed.z);

		newAABB.max.x = (std::max)(newAABB.max.x, transformed.x);
		newAABB.max.y = (std::max)(newAABB.max.y, transformed.y);
		newAABB.max.z = (std::max)(newAABB.max.z, transformed.z);
	}

	return newAABB;
}

Vector3 AABB::center() const {
	Vector3 ret;
	ret.x = (min.x + max.x) / 2.0f;
	ret.y = (min.y + max.y) / 2.0f;
	ret.z = (min.z + max.z) / 2.0f;
	return ret;
}

void AABB::Initialize() {
	min.x = -0.5f;
	min.y = -0.5f;
	min.z = -0.5f;

	max.x = 0.5f;
	max.y = 0.5f;
	max.z = 0.5f;
}

void AABB::Expand(const Vector3& point) {
	min.x = (std::min)(min.x, point.x);
	min.y = (std::min)(min.y, point.y);
	min.z = (std::min)(min.z, point.z);

	max.x = (std::max)(max.x, point.x);
	max.y = (std::max)(max.y, point.y);
	max.z = (std::max)(max.z, point.z);
}

void AABB::Expand(const AABB& other) {
	Expand(other.min);
	Expand(other.max);
}

Vector3 AABB::GetSize() const {
	return { max.x - min.x, max.y - min.y, max.z - min.z };
}

void AABB::ResetToExtreme() {
	min = { FLT_MAX, FLT_MAX, FLT_MAX };
	max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
}