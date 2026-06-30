#pragma once
#include "Structures.h"

struct Ray {
    Vector3 origin;// 発射位置
    Vector3 direction;// 進む方向：スピード
    Vector3 inverseDirection;// 逆数：計算で使う
    Ray(Vector3 origin, Vector3 direction) {
        origin = origin;
        direction = direction;
        // 割り算を掛け算にするための事前計算
        inverseDirection = Vector3(
            1.0f / direction.x,
            1.0f / direction.y,
            1.0f / direction.z
        );
    }
};