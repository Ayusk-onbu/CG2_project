#include "PhysicsTriangle.h"

AABB PhysicsTriangle::GetAABB()const {
    AABB bounds;
    bounds.ResetToExtreme(); // 極小状態からスタート
    bounds.Expand(v0);
    bounds.Expand(v1);
    bounds.Expand(v2);
    return bounds;
}

Vector3 PhysicsTriangle::GetCenter()const {
    return{
        (v0.x + v1.x + v2.x) / 3.0f,
        (v0.y + v1.y + v2.y) / 3.0f,
        (v0.z + v1.z + v2.z) / 3.0f
    };
}

inline PhysicsTriangle TransformTriangle(const PhysicsTriangle& tri, const Matrix4x4& mat) {
    PhysicsTriangle worldTri = tri;

    // 頂点をワールド座標へ変換
    Vector4 v0 = Matrix4x4::Transform(mat, Vector4{ tri.v0.x, tri.v0.y, tri.v0.z, 1.0f });
    Vector4 v1 = Matrix4x4::Transform(mat, Vector4{ tri.v1.x, tri.v1.y, tri.v1.z, 1.0f });
    Vector4 v2 = Matrix4x4::Transform(mat, Vector4{ tri.v2.x, tri.v2.y, tri.v2.z, 1.0f });

    worldTri.v0 = { v0.x, v0.y, v0.z };
    worldTri.v1 = { v1.x, v1.y, v1.z };
    worldTri.v2 = { v2.x, v2.y, v2.z };

    return worldTri;
}