#pragma once
#include "UIContainer.h"

class UIEditor {
public:
	static UIEditor* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<UIEditor>();
		}
		return instance_.get();
	}
public:
	void Update();
public:
	void SetTargetElement(UIElement* element) { targetElement_ = element; }
	void SetActiveAnimation(UIHAnimation* animation) { activeAnimation_ = animation; }
private:
	void DrawAnimationEditor();
	template<typename T>
	void DrawTrackInfo(const char* label, UIHAnimationTrack<T>& track);
	template<typename T>
	void DrawTimelineTrack(const char* label, UIHAnimationTrack<T>& track, float maxTime);
private:
	static std::unique_ptr<UIEditor>instance_;
	// [ ***編集・表示用データ*** ]
	// [ タイムラインのシーク位置 ]
	float seekTime_ = 0.0f;
	// [ 再生ロジック ]
	bool isPlaying_ = false;
	float playbackSpeed_ = 1.0f;
	// [選択中のイージングタイプ ]
	int selectedEase_ = 0;
	// [ 選択中のアニメーション名 ]
	std::string selectedAnimationName_;
	// [ 選択中のUI ]
	UIElement* targetElement_ = nullptr;
	// [ 編集中のアニメーション ]
	UIHAnimation* activeAnimation_ = nullptr;
	// [ 編集中のアニメーションの名前 ]
	std::string inputAnimationName_;
	// [ イージングタイプの名前一覧 ]
	static const char* easingNames[];
};