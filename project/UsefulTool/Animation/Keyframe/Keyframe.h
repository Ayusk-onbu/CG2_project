#pragma once
#include "Structures.h"
#include "Quaternion.h"

template <typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector4 = Keyframe<Vector4>;
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeVector2 = Keyframe<Vector2>;
using KeyframeQuaternion = Keyframe<Quaternion>;
using KeyframeFloat = Keyframe<float>;


