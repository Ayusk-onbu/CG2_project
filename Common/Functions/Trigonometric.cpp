#include "Trigonometric.h"

float cot(float theta) {
	float ret = 1.0f / std::tan(theta);
	return ret;
}

float Deg2Rad(float deg) {
	float ret = static_cast<float>(deg) * (3.14159265358979323846f / 180.0f);
	return ret;
}