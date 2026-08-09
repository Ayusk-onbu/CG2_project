#pragma once
#include "Structures.h"
#include "Keyframe.h"
#include "ModelData.h"
#include <vector>
#include <map>
#include <iostream>

template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>>keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion>rotate;
	AnimationCurve<Vector3> scale;
};

class Animation {
public:
	/// <summary>
	/// Animationのファイルを読み込む
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	void LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// Animationを再生する
	/// </summary>
	/// <returns></returns>
	Matrix4x4 Update(const std::string& name);

	void ApplyAnimation(Skeleton& skeleton);

	void SetIsLoop(bool flag) { isLoop_ = flag; }

	void TimeFlow();
public:
	// 時間関係
	// [ アニメーション全体の尺(単位は秒) ]
	float duration_;
	// [ 再生中の時間 ]
	float animationTime_ = 0.0f;

	// NodeAnimationの集合。Node名で引けるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations_;

private:
	// [ ループの有無 ] 
	bool isLoop_ = false;
};

// --------------------------------------------------
// 1つのアニメーションデータ（アセット / 読み取り専用）
// --------------------------------------------------
class AnimationClip {
public:
	/// <summary>
	/// 指定した時刻 (time) の姿勢を Skeleton に適用する
	/// </summary>
	/// <param name="skeleton">適用対象のスケルトン</param>
	/// <param name="time">現在の再生時間(秒)</param>
	void ApplyToSkeleton(Skeleton& skeleton, float time) const;

public:
	std::string name_;      // アニメーション名 (例: "Take 001", "Walk")
	float duration_ = 0.0f; // 全体の尺 (秒)

	// Node名 (Joint名) ごとのキーフレーム集合
	std::map<std::string, NodeAnimation> nodeAnimations_;
};