#pragma once
#include "ISingleton.h"
#include "Animation.h"
#include <unordered_map>

class AnimationManager : public ISingleton<AnimationManager>
{
public:
	friend class ISingleton<AnimationManager>;

public:
	/// <summary>
	/// ファイル内に存在する「すべて」のアニメーションをロードする
	/// </summary>
	/// <returns>ロードされたアニメーション名(ID)のリスト</returns>
	std::vector<std::string> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// ロード済みのアニメーションを取得する
	/// </summary>
	const AnimationClip* GetAnimation(const std::string& animationName) const;

	/// <summary>
	/// 登録されているかチェック
	/// </summary>
	bool HasAnimation(const std::string& animationName) const;

	// 登録されているアニメーション名の一覧を取得
	std::vector<std::string> GetAnimationNames() const;
private:
	// アニメーション識別名 (例: "Player_Walk") -> AnimationClip
	std::unordered_map<std::string, AnimationClip> animations_;
};