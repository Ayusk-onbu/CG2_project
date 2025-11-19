#pragma once
#include "Structures.h"
#include "EasingType.h"
//イージングの種類（使いたいときは関数の引数にこちらを指定）

void Easing(Vector3& Pos, const Vector3& startPos, const Vector3& endPos, float nowFrame, const float& endFrame,EASINGTYPE easeType, float magNum = 1.0f);
float Easing_Float(const float& startValue, const float& endValue, float nowFrame, const float& endFrame, EASINGTYPE easeType, float magNum = 1.0f);

