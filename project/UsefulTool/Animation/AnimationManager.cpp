#include "AnimationManager.h"
#include "Log.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

std::vector<std::string> AnimationManager::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;

	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene && scene->mNumAnimations != 0 && "Animation file loading failed or has no animations.");

	std::vector<std::string> loadedNames;

	// ファイルに含まれる「すべて」のアニメーションをループで解析・登録する
	for (uint32_t animIdx = 0; animIdx < scene->mNumAnimations; ++animIdx) {
		aiAnimation* animationAssimp = scene->mAnimations[animIdx];

		AnimationClip clip;
		clip.name_ = animationAssimp->mName.C_Str();

		// アニメーション名が空の場合はファイル名+インデックスで命名
		if (clip.name_.empty()) {
			clip.name_ = filename + "_" + std::to_string(animIdx);
		}

		// 尺の計算 (秒単位)
		float ticksPerSecond = static_cast<float>(animationAssimp->mTicksPerSecond != 0 ? animationAssimp->mTicksPerSecond : 25.0f);
		clip.duration_ = static_cast<float>(animationAssimp->mDuration / ticksPerSecond);

		// 各チャンネル (ボーンごとのトラック) を解析
		for (uint32_t channelIdx = 0; channelIdx < animationAssimp->mNumChannels; ++channelIdx) {
			aiNodeAnim* nodeAnimAssimp = animationAssimp->mChannels[channelIdx];
			NodeAnimation& nodeAnimation = clip.nodeAnimations_[nodeAnimAssimp->mNodeName.C_Str()];

			// 1. Position キーフレーム
			for (uint32_t kIdx = 0; kIdx < nodeAnimAssimp->mNumPositionKeys; ++kIdx) {
				aiVectorKey& keyAssimp = nodeAnimAssimp->mPositionKeys[kIdx];
				KeyframeVector3 keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / ticksPerSecond);
				keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手->左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			// 2. Rotation キーフレーム
			for (uint32_t kIdx = 0; kIdx < nodeAnimAssimp->mNumRotationKeys; ++kIdx) {
				aiQuatKey& keyAssimp = nodeAnimAssimp->mRotationKeys[kIdx];
				KeyframeQuaternion keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / ticksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手->左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}

			// 3. Scale キーフレーム
			for (uint32_t kIdx = 0; kIdx < nodeAnimAssimp->mNumScalingKeys; ++kIdx) {
				aiVectorKey& keyAssimp = nodeAnimAssimp->mScalingKeys[kIdx];
				KeyframeVector3 keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / ticksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		// マネージャーへ登録
		std::string animID = clip.name_;
		animations_[animID] = std::move(clip);
		loadedNames.push_back(animID);

		Log::View("Loaded Animation: " + animID);
	}

	return loadedNames;
}

const AnimationClip* AnimationManager::GetAnimation(const std::string& animationName) const {
	auto it = animations_.find(animationName);
	if (it != animations_.end()) {
		return &it->second;
	}
	Log::View("Animation Not Found: " + animationName);
	return nullptr;
}

bool AnimationManager::HasAnimation(const std::string& animationName) const {
	return animations_.find(animationName) != animations_.end();
}

std::vector<std::string> AnimationManager::GetAnimationNames() const {
	std::vector<std::string> names;
	names.reserve(animations_.size());

	for (const auto& [name, _] : animations_) {
		names.push_back(name);
	}

	return names;
}