#include "Easing.h"
#include <cmath>
#include <numbers>

void Easing(Vector3& Pos, const Vector3& startPos, const Vector3& endPos, float nowFrame,const float& endFrame,EASINGTYPE easeType, float magNum){

	float x = nowFrame / endFrame;

	if (easeType == EASINGTYPE::None) {x = x;}
	if (easeType == EASINGTYPE::InSine) {x = InSine(x);}
	if (easeType == EASINGTYPE::OutSine) {x = OutSine(x);}
	if (easeType == EASINGTYPE::InOutSine) {x = InOutSine(x);}
	if (easeType == EASINGTYPE::InQuad) {x = InQuad(x);}
	if (easeType == EASINGTYPE::OutQuad) {x = OutQuad(x);}
	if (easeType == EASINGTYPE::InOutQuad) {x = InOutQuad(x);}
	if (easeType == EASINGTYPE::InCubic) {x = InCubic(x);}
	if (easeType == EASINGTYPE::OutCubic) {x = OutCubic(x);}
	if (easeType == EASINGTYPE::InOutCubic) {x = InOutCubic(x);}
	if (easeType == EASINGTYPE::InQuart) {x = InQuart(x);}
	if (easeType == EASINGTYPE::OutQuart) {x = OutQuart(x);}
	if (easeType == EASINGTYPE::InOutQuart) {x = InOutQuart(x);}
	if (easeType == EASINGTYPE::InQuint) {x = InQuint(x);}
	if (easeType == EASINGTYPE::OutQuint) {x = OutQuint(x);}
	if (easeType == EASINGTYPE::InOutQuint) {x = InOutQuint(x);}
	if (easeType == EASINGTYPE::InExpo) {x = InExpo(x);}
	if (easeType == EASINGTYPE::OutExpo) {x = OutExpo(x);}
	if (easeType == EASINGTYPE::InOutExpo) {x = InOutExpo(x);}
	if (easeType == EASINGTYPE::InCirc) {x = InCirc(x);}
	if (easeType == EASINGTYPE::OutCirc) {x = OutCirc(x);}
	if (easeType == EASINGTYPE::InOutCirc) {x = InOutCirc(x);}
	if (easeType == EASINGTYPE::InBack) {x = InBack(x);}
	if (easeType == EASINGTYPE::OutBack) {x = OutBack(x);}
	if (easeType == EASINGTYPE::InOutBack) {x = InOutBack(x);}
	if (easeType == EASINGTYPE::InElastic) {x = InElastic(x);}
	if (easeType == EASINGTYPE::OutElastic) {x = OutElastic(x);}
	if (easeType == EASINGTYPE::InOutElastic) {x = InOutElastic(x);}
	if (easeType == EASINGTYPE::InBounce) {x = InBounce(x);}
	if (easeType == EASINGTYPE::OutBounce) {x = OutBounce(x);}
	if (easeType == EASINGTYPE::InOutBounce) {x = InOutBounce(x);}

	x = std::powf(x, magNum);

	Pos.x = float(startPos.x * (1 - x) + endPos.x * x);
	Pos.y = float(startPos.y * (1 - x) + endPos.y * x);
	Pos.z = float(startPos.z * (1 - x) + endPos.z * x);
}