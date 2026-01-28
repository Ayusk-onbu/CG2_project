#include "Animation.h"
#include <cassert>
#include "MathUtils.h"
#include "ModelManager.h"

// 外部ファイルのInclude
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ---------------------
// Animation の読み込みに関して
// ---------------------

void Animation::LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {

	Assimp::Importer importer;
	// Animationフォルダを読み込むため、パスを作成する
	std::string filePath = directoryPath + "/" + filename;

	// 
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	// アニメーションがない
	assert(scene->mNumAnimations != 0);

	// 最初のアニメーションだけ採用(後々複数対応すると良い)
	aiAnimation* animationAssimp = scene->mAnimations[0];

	// 時間の単位を秒に変換
	// mTickParSecond：周波数
	// mDuration：mTicksParSecondで指定された周波数における長さ
	duration_ = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0;channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		// 
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];

		// Animation名前を受け取り操作する準備
		NodeAnimation& nodeAnimation = nodeAnimations_[nodeAnimationAssimp->mNodeName.C_Str()];

		// Position
		for (uint32_t keyIndex = 0;keyIndex < nodeAnimationAssimp->mNumPositionKeys;++keyIndex) {
			
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;

			// 秒に変換
			keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);

			// 右手座標系から左手座標系に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };

			// 情報を保存
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		// Rotate
		for (uint32_t keyIndex = 0;keyIndex < nodeAnimationAssimp->mNumRotationKeys;++keyIndex) {

			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;

			// 秒に変換
			keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);

			// 右手座標系から左手座標系に変換
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y , -keyAssimp.mValue.z, keyAssimp.mValue.w };

			// 情報を保存する
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		// Scale
		for (uint32_t keyIndex = 0;keyIndex < nodeAnimationAssimp->mNumScalingKeys;++keyIndex) {

			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;

			// 秒に変換
			keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);

			// 右手座標系から左手座標系に変換しなくて大丈夫
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y ,keyAssimp.mValue.z };

			// 情報を保存
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
	// 解析完了
	return;
}

// --------------------
// Animationを進める
// --------------------

Matrix4x4 Animation::Update(const std::string& name) {

	// 時刻を進める(将来的にゲームの内の実際のdelteTimeを書く)
	animationTime_ += 1.0f / 60.0f;

	// ループフラグが有効なら
	if (isLoop_ == true) {
		// 最後まで行ったら時間を最初に戻す
		animationTime_ = std::fmod(animationTime_, duration_);
	}

	ModelData& model = ModelManager::GetInstance()->LoadModelData(name);

	// NodeのAnimationを取得
	NodeAnimation& rootNodeAnimation = nodeAnimations_[model.rootNode.name];

	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);

	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);

	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

	Matrix4x4 scaleMat = Matrix4x4::Make::Scale(scale);
	Matrix4x4 rotationMat = Quaternion::MakeRotateMatrix(rotate);
	Matrix4x4 translateMat = Matrix4x4::Make::Translate(translate);

	// S * R * T
	Matrix4x4 localMatrix;
	localMatrix = Matrix4x4::Multiply(scaleMat, rotationMat);
	localMatrix = Matrix4x4::Multiply(localMatrix, translateMat);
	
	return localMatrix;
}

// --------------------------------
// Animation を Skeleton に適用する
// --------------------------------

void Animation::ApplyAnimation(Skeleton& skeleton) {
	// 適用
	for (Joint& joint : skeleton.joints_) {
		if (auto it = nodeAnimations_.find(joint.name); it != nodeAnimations_.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.transform_.translation_ = (CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_));
			joint.transform.quaternion_ = (CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_));
			joint.transform.transform_.scale_ = (CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_));
			joint.transform.isDirty_ = true;
			joint.transform.LocalToWorld();
		}
	}
	//ImGuiManager::GetInstance()->DrawDrag("Animation Time", animationTime_);
}

void Animation::TimeFlow() { 
	animationTime_ += 1.0f / 60.0f; 
	// ループフラグが有効なら
	if (isLoop_ == true) {
		// 最後まで行ったら時間を最初に戻す
		animationTime_ = std::fmod(animationTime_, duration_);
	}
}