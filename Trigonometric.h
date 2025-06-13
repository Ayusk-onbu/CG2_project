#pragma once
#include <cmath>

static float cot(float theta) {
	float ret = 1.0f / std::tanf(theta);
	return ret;
}