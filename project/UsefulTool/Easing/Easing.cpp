#include "Easing.h"

#define _USE_MATH_DEFINES

const float pai = 3.1415f;


float InSine(float x) {

	x = 1 - cosf((x * float(pai)) / 2);

	return x;
}

float OutSine(float x) {

	x = sinf((x * float(pai)) / 2.0f);

	return x;
}

float InOutSine(float x) {

	x = -(cosf(float(pai) * x) - 1.0f) / 2.0f;

	return x;
}

float InQuad(float x) {

	x = x * x;

	return x;
}

float OutQuad(float x) {

	x = 1.0f - (1.0f - x) * (1.0f - x);

	return x;
}

float InOutQuad(float x) {

	if (x < 0.5f)
		x = 2.0f * x * x;
	else
		x = 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	return x;
}

float InCubic(float x) {

	x = x * x * x;

	return x;
}

float OutCubic(float x) {

	x = 1.0f - powf(1.0f - x, 3.0f);

	return x;
}

float InOutCubic(float x) {

	if (x < 0.5f)
		x = 4.0f * x * x * x;
	else
		x = 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	return x;
}

float InQuart(float x) {

	x = x * x * x * x;

	return x;
}

float OutQuart(float x) {

	x = 1 - powf(1.0f - x, 4.0f);

	return x;
}

float InOutQuart(float x) {

	if (x < 0.5f)
		x = 8.0f * x * x * x * x;
	else
		x = 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	return x;
}

float InQuint(float x) {

	x = x * x * x * x * x;

	return x;
}

float OutQuint(float x) {

	x = 1 - powf(1 - x, 5);

	return x;
}

float InOutQuint(float x) {

	if (x < 0.5f)
		x = 16.0f * x * x * x * x * x;
	else
		x = 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	return x;
}

float InExpo(float x) {

	x = powf(2.0f, 10.0f * x - 10.0f);

	return x;
}

float OutExpo(float x) {

	x = 1.0f - powf(2.0f, -10.0f * x);

	return x;
}

float InOutExpo(float x) {

	if (x < 0.5f)
		x = powf(2.0f, 20.0f * x - 10.0f) / 2.0f;
	else
		x = (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	return x;
}

float InCirc(float x) {

	x = 1.0f - sqrtf(1.0f - powf(x, 2.0f));

	return x;
}

float OutCirc(float x) {

	x = sqrtf(1.0f - powf(x - 1.0f, 2.0f));

	return x;
}

float InOutCirc(float x) {

	if (x < 0.5f)
		x = (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f;
	else
		x = (sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	return x;
}

float InBack(float x) {

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	x = c3 * x * x * x - c1 * x * x;

	return x;
}

float OutBack(float x) {

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	x = 1 + c3 * powf(x - 1, 3) + c1 * powf(x - 1, 2);

	return x;
}

float InOutBack(float x) {

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	if (x < 0.5f)
		x = (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f;
	else
		x = (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	return x;
}

float InElastic(float x) {

	const float c4 = (2.0f * float(pai)) / 3.0f;

	x = -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);

	return x;
}

float OutElastic(float x) {

	const float c4 = (2 * float(pai) / 3);

	x = powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;

	return x;
}

float InOutElastic(float x) {

	const float c5 = (2.0f * float(pai)) / 4.5f;

	if (x < 0.5f)
		x = -(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f;
	else
		x = (powf(2.0f, -20.0f * x + 10.0f) * sinf(20.0f * x - 11.125f) * c5) / 2.0f + 1.0f;

	return x;
}

float OutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1) {
		x = n1 * x * x;
	}
	else if (x < 2 / d1) {
		x = n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		x = n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		x = n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}

	return x;
}

float InBounce(float x) {

	x = 1.0f - OutBounce(1 - x);

	return x;
}

float InOutBounce(float x) {

	if (x < 0.5f)
		x = (1.0f - OutBounce(1.0f - 2.0f * x)) / 2.0f;
	else
		x = (1.0f + OutBounce(2.0f * x - 1.0f)) / 2.0f;

	return x;
}

void Easing(Vector2* Pos, float startPosX, float startPosY, float endPosX, float endPosY, float* nowFrame, float endFrame, float* t, float magNum, int material) {

	*t = *nowFrame / endFrame;

	float x = *t;

	if (material == materialNone) {
		x = x;
	}

	if (material == materialInSine) {
		x = InSine(x);
	}

	if (material == materialOutSine) {
		x = OutSine(x);
	}

	if (material == materialInOutSine) {
		x = InOutSine(x);
	}

	if (material == materialInQuad) {
		x = InQuad(x);
	}

	if (material == materialOutQuad) {
		x = OutQuad(x);
	}

	if (material == materialInOutQuad) {
		x = InOutQuad(x);
	}

	if (material == materialInCubic) {
		x = InCubic(x);
	}

	if (material == materialOutCubic) {
		x = OutCubic(x);
	}

	if (material == materialInOutCubic) {
		x = InOutCubic(x);
	}

	if (material == materialInQuart) {
		x = InQuart(x);
	}

	if (material == materialOutQuart) {
		x = OutQuart(x);
	}

	if (material == materialInOutQuart) {
		x = InOutQuart(x);
	}

	if (material == materialInQuint) {
		x = InQuint(x);
	}

	if (material == materialOutQuint) {
		x = OutQuint(x);
	}

	if (material == materialInOutQuint) {
		x = InOutQuint(x);
	}

	if (material == materialInExpo) {
		x = InExpo(x);
	}

	if (material == materialOutExpo) {
		x = OutExpo(x);
	}

	if (material == materialInOutExpo) {
		x = InOutExpo(x);
	}

	if (material == materialInCirc) {
		x = InCirc(x);
	}

	if (material == materialOutCirc) {
		x = OutCirc(x);
	}

	if (material == materialInOutCirc) {
		x = InOutCirc(x);
	}

	if (material == materialInBack) {
		x = InBack(x);
	}

	if (material == materialOutBack) {
		x = OutBack(x);
	}

	if (material == materialInOutBack) {
		x = InOutBack(x);
	}

	if (material == materialInElastic) {
		x = InElastic(x);
	}

	if (material == materialOutElastic) {
		x = OutElastic(x);
	}

	if (material == materialInOutElastic) {
		x = InOutElastic(x);
	}

	if (material == materialInBounce) {
		x = InBounce(x);
	}

	if (material == materialOutBounce) {
		x = OutBounce(x);
	}

	if (material == materialInOutBounce) {
		x = InOutBounce(x);
	}

	x = powf(x, magNum);

	Pos->x = float(startPosX * (1 - x) + endPosX * x);
	Pos->y = float(startPosY * (1 - x) + endPosY * x);

	*nowFrame += 1;
}

void Easing(Vector3& Pos, const Vector3& startPos, const Vector3& endPos, float nowFrame, float endFrame, float magNum, int material){

	float x = nowFrame / endFrame;

	if (material == materialNone) {
		x = x;
	}

	if (material == materialInSine) {
		x = InSine(x);
	}

	if (material == materialOutSine) {
		x = OutSine(x);
	}

	if (material == materialInOutSine) {
		x = InOutSine(x);
	}

	if (material == materialInQuad) {
		x = InQuad(x);
	}

	if (material == materialOutQuad) {
		x = OutQuad(x);
	}

	if (material == materialInOutQuad) {
		x = InOutQuad(x);
	}

	if (material == materialInCubic) {
		x = InCubic(x);
	}

	if (material == materialOutCubic) {
		x = OutCubic(x);
	}

	if (material == materialInOutCubic) {
		x = InOutCubic(x);
	}

	if (material == materialInQuart) {
		x = InQuart(x);
	}

	if (material == materialOutQuart) {
		x = OutQuart(x);
	}

	if (material == materialInOutQuart) {
		x = InOutQuart(x);
	}

	if (material == materialInQuint) {
		x = InQuint(x);
	}

	if (material == materialOutQuint) {
		x = OutQuint(x);
	}

	if (material == materialInOutQuint) {
		x = InOutQuint(x);
	}

	if (material == materialInExpo) {
		x = InExpo(x);
	}

	if (material == materialOutExpo) {
		x = OutExpo(x);
	}

	if (material == materialInOutExpo) {
		x = InOutExpo(x);
	}

	if (material == materialInCirc) {
		x = InCirc(x);
	}

	if (material == materialOutCirc) {
		x = OutCirc(x);
	}

	if (material == materialInOutCirc) {
		x = InOutCirc(x);
	}

	if (material == materialInBack) {
		x = InBack(x);
	}

	if (material == materialOutBack) {
		x = OutBack(x);
	}

	if (material == materialInOutBack) {
		x = InOutBack(x);
	}

	if (material == materialInElastic) {
		x = InElastic(x);
	}

	if (material == materialOutElastic) {
		x = OutElastic(x);
	}

	if (material == materialInOutElastic) {
		x = InOutElastic(x);
	}

	if (material == materialInBounce) {
		x = InBounce(x);
	}

	if (material == materialOutBounce) {
		x = OutBounce(x);
	}

	if (material == materialInOutBounce) {
		x = InOutBounce(x);
	}

	x = powf(x, magNum);

	Pos.x = float(startPos.x * (1 - x) + endPos.x * x);
	Pos.y = float(startPos.y * (1 - x) + endPos.y * x);
	Pos.z = float(startPos.z * (1 - x) + endPos.z * x);

}

//ｔとかの初期化関数
void EasingInitialize(float* t, float* NowFrame, Vector2* startPos, Vector2* Ini_startPos) {

	*startPos = *Ini_startPos;
	*t = 0.0f;
	*NowFrame = 0.0f;
}

float easeBounce(float start, float end, float x)
{
	float result;

	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1) {
		x = n1 * x * x;
	}
	else if (x < 2 / d1) {
		x = n1 * (x -= 1.5f / d1) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		x = n1 * (x -= 2.25f / d1) * x + 0.9375f;
	}
	else {
		x = n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}

	result = (1.0f - x) * start + end * x;

	return result;
}