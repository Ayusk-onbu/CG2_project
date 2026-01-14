#include "MathUtils.h"
#include <algorithm>
#include <cmath>
#include <cassert>

Vector3 TransformNormal(Vector3& v, Matrix4x4& m) {
	Vector3 ret{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};
	return ret;
}

float Lerp(float v1, float v2, float t) {
	return v1 + (v2 - v1) * t;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return {
		Lerp(v1.x,v2.x,t),
		Lerp(v1.y,v2.y,t),
		Lerp(v1.z,v2.z,t)
	};
}

Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t) {
	return {
		Lerp(q1.x,q2.x,t),
		Lerp(q1.y,q2.y,t),
		Lerp(q1.z,q2.z,t),
		Lerp(q1.w,q2.w,t)
	};
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	Vector3 v1N = Normalize(v1);
	Vector3 v2N = Normalize(v2);

	float dot = Dot(v1N, v2N);

	dot = std::clamp(dot, -0.9999f, 1.0f);

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1.0f - t) * theta);
	float sinThetaTo = std::sin(t * theta);
	Vector3 normaLerpV;
	if (sinTheta < 1.0e-5) {
		normaLerpV = v1N;
	}
	else {
		normaLerpV = (sinThetaFrom * v1N + sinThetaTo * v2N) / sinTheta;
	}
	
	float length1 = Length(v1);
	float length2 = Length(v2);
	
	float length = Lerp(length1, length2, t);

	return length * normaLerpV;
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	const float s = 0.5f;
	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 e3 = -p0 + 3.0f * p1 - 3.0f * p2 + p3;
	Vector3 e2 = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2.0f * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数は制御点の数-1
	size_t division = points.size() - 1;
	// 1区間の長さ（全体を1.0とした割合）
	float areaWidth = 1.0f / division;

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::min(index,division - 1);

	// 区間内の始点を0.0f、終点を1.0fとしたときの現在位置
	float t_2 = (t - areaWidth * index)/areaWidth;
	// 下限(0.0f)と上限(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 先頭もしくは最後の場合の処理
	if (index == 0) {
		index0 = index1;
	}
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 4点を指定してCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

#pragma region Distance
float Distance(const Vector2& a, const Vector2& b) {
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
#pragma endregion

// --------------------------------
// AnimationTimeによる計算
// --------------------------------

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	// キーがあるか確認する
	assert(!keyframes.empty());

	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		// キーが１つもしくは、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// index と nextIndex の二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまできたらドンマイやね
	return (*keyframes.rbegin()).value;
}

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	// キーがあるか確認する
	assert(!keyframes.empty());

	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		// キーが１つもしくは、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// index と nextIndex の二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Quaternion::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまできたらドンマイやね
	return (*keyframes.rbegin()).value;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	Matrix4x4 scaleMat = Matrix4x4::Make::Scale(scale);
	Matrix4x4 rotationMat = Quaternion::MakeRotateMatrix(rotate);
	Matrix4x4 translateMat = Matrix4x4::Make::Translate(translate);

	// S * R * T
	Matrix4x4 localMatrix;
	localMatrix = Matrix4x4::Multiply(scaleMat, rotationMat);
	localMatrix = Matrix4x4::Multiply(localMatrix, translateMat);

	return localMatrix;
}