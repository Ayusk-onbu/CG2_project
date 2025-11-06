#include "EasingType.h"
#include <cmath>
#include <numbers>


float InSine(float x) {

	x = 1.0f - std::cos((x * float(std::numbers::pi)) / 2);

	return x;
}

float OutSine(float x) {

	x = std::sin((x * float(std::numbers::pi)) / 2.0f);

	return x;
}

float InOutSine(float x) {

	x = -(std::cos(float(std::numbers::pi) * x) - 1.0f) / 2.0f;

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
		x = 1.0f - std::powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	return x;
}

float InCubic(float x) {

	x = x * x * x;

	return x;
}

float OutCubic(float x) {

	x = 1.0f - std::powf(1.0f - x, 3.0f);

	return x;
}

float InOutCubic(float x) {

	if (x < 0.5f)
		x = 4.0f * x * x * x;
	else
		x = 1.0f - std::powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	return x;
}

float InQuart(float x) {

	x = x * x * x * x;

	return x;
}

float OutQuart(float x) {

	x = 1.0f - std::powf(1.0f - x, 4.0f);

	return x;
}

float InOutQuart(float x) {

	if (x < 0.5f)
		x = 8.0f * x * x * x * x;
	else
		x = 1.0f - std::powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	return x;
}

float InQuint(float x) {

	x = x * x * x * x * x;

	return x;
}

float OutQuint(float x) {

	x = 1.0f - std::powf(1 - x, 5);

	return x;
}

float InOutQuint(float x) {

	if (x < 0.5f)
		x = 16.0f * x * x * x * x * x;
	else
		x = 1.0f - std::powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	return x;
}

float InExpo(float x) {

	x = std::powf(2.0f, 10.0f * x - 10.0f);

	return x;
}

float OutExpo(float x) {

	x = 1.0f - std::powf(2.0f, -10.0f * x);

	return x;
}

float InOutExpo(float x) {

	if (x < 0.5f)
		x = std::powf(2.0f, 20.0f * x - 10.0f) / 2.0f;
	else
		x = (2.0f - std::powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	return x;
}

float InCirc(float x) {

	x = 1.0f - sqrtf(1.0f - std::powf(x, 2.0f));

	return x;
}

float OutCirc(float x) {

	x = sqrtf(1.0f - std::powf(x - 1.0f, 2.0f));

	return x;
}

float InOutCirc(float x) {

	if (x < 0.5f)
		x = (1.0f - sqrtf(1.0f - std::powf(2.0f * x, 2.0f))) / 2.0f;
	else
		x = (sqrtf(1.0f - std::powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

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

	x = 1 + c3 * std::powf(x - 1, 3) + c1 * std::powf(x - 1, 2);

	return x;
}

float InOutBack(float x) {

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	if (x < 0.5f)
		x = (std::powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f;
	else
		x = (std::powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	return x;
}

float InElastic(float x) {

	const float c4 = (2.0f * float(std::numbers::pi)) / 3.0f;

	x = -std::powf(2.0f, 10.0f * x - 10.0f) * std::sin((x * 10.0f - 10.75f) * c4);

	return x;
}

float OutElastic(float x) {

	const float c4 = (2 * float(std::numbers::pi) / 3);

	x = std::powf(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;

	return x;
}

float InOutElastic(float x) {

	const float c5 = (2.0f * float(std::numbers::pi)) / 4.5f;

	if (x < 0.5f)
		x = -(std::powf(2.0f, 20.0f * x - 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f;
	else
		x = (std::powf(2.0f, -20.0f * x + 10.0f) * std::sin(20.0f * x - 11.125f) * c5) / 2.0f + 1.0f;

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