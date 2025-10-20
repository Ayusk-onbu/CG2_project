#pragma once
#include "Structures.h"

//イージングの種類（使いたいときは関数の引数にこちらを指定）
enum Material {
	materialNone,

	materialInSine,

	materialOutSine,

	materialInOutSine,

	materialInQuad,

	materialOutQuad,

	materialInOutQuad,

	materialInCubic,

	materialOutCubic,

	materialInOutCubic,

	materialInQuart,

	materialOutQuart,

	materialInOutQuart,

	materialInQuint,

	materialOutQuint,

	materialInOutQuint,

	materialInExpo,

	materialOutExpo,

	materialInOutExpo,

	materialInCirc,

	materialOutCirc,

	materialInOutCirc,

	materialInBack,

	materialOutBack,

	materialInOutBack,

	materialInElastic,

	materialOutElastic,

	materialInOutElastic,

	materialInBounce,

	materialOutBounce,

	materialInOutBounce,
};

void Easing(Vector2* Pos, float startPosX, float startPosY, float endPosX, float endPosY, float* nowFrame, float endFrame, float* t, float magNum, int material);
void Easing(Vector3& Pos, const Vector3& startPos, const Vector3& endPos, float nowFrame, float endFrame, float magNum, int material);

//ｔとかの初期化関数
void EasingInitialize(float* t, float* NowFrame, Vector2* startPos, Vector2* Ini_startPos);

float InSine(float x);

float OutSine(float x);

float InOutSine(float x);

float InQuad(float x);

float OutQuad(float x);

float InOutQuad(float x);

float InCubic(float x);

float OutCubic(float x);

float InOutCubic(float x);

float InQuart(float x);

float OutQuart(float x);

float InOutQuart(float x);

float InQuint(float x);

float OutQuint(float x);

float InOutQuint(float x);

float InExpo(float x);

float OutExpo(float x);

float InOutExpo(float x);

float InCirc(float x);

float OutCirc(float x);

float InOutCirc(float x);

float InBack(float x);

float OutBack(float x);

float InOutBack(float x);

float InElastic(float x);

float OutElastic(float x);

float InOutElastic(float x);

float InBounce(float x);

float OutBounce(float x);

float InOutBounce(float x);

void EasingInitialize(float* t, float* NowFrame, Vector2* startPos, Vector2* Ini_startPos);

float easeBounce(float start, float end, float x);

