#pragma once
#include "WorldTransform.h"

struct ParticleData {
	WorldTransform worldTransform;
	Vector3 velocity;
	Vector4 color;
	Vector4 startColor;
	Vector4 endColor;
	float startScale;
	float endScale;
	float lifeTime;
	float currentTime;
};