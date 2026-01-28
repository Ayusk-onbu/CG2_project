#include "UIHAnimation.h"
#include "UIElement.h"
#include <iostream>

std::unique_ptr<UIHAnimationManager> UIHAnimationManager::instance_ = nullptr;

void UIHAnimation::Apply(UIElement* element, float currentTime) {
	if (element == nullptr)return;

	// Objの反映⑦[ Apply ]
	// [ 座標 ]
	if (posTrack_.IsEmpty() == false) {
		element->GetObj().worldTransform_.transform_.translation_ =
			posTrack_.Evaluate(currentTime);
	}
	// [ スケール ]
	if (scaleTrack_.IsEmpty() == false) {
		element->GetObj().worldTransform_.transform_.scale_ =
			scaleTrack_.Evaluate(currentTime);
	}
	// [ 回転 ]
	if (rotateTrack_.IsEmpty() == false) {
		element->GetObj().worldTransform_.set_.Rotation(
			rotateTrack_.Evaluate(currentTime));
	}
	// [ 色 ]
	if (colorTrack_.IsEmpty() == false) {
		element->GetObj().SetColor(
			colorTrack_.Evaluate(currentTime));
	}
	// ここからどんどん追加する
}

void UIHAnimation::Load(const json& j) {
	name = j.value("name", "NewAnimation");
	// ③[ Load ]->④[ Save ]
	if (j.contains("posTrack")) posTrack_.Load(j.at("posTrack"));
	if (j.contains("scaleTrack")) scaleTrack_.Load(j.at("scaleTrack"));
	if (j.contains("rotateTrack")) rotateTrack_.Load(j.at("rotateTrack"));
	if (j.contains("colorTrack")) colorTrack_.Load(j.at("colorTrack"));
}
// Save
void UIHAnimation::Save() {
	json j;
	j["name"] = name;
	// ④[ Save ]->⑤[ 値を更新 ]
	posTrack_.Save(j["posTrack"]);
	scaleTrack_.Save(j["scaleTrack"]);
	rotateTrack_.Save(j["rotateTrack"]);
	colorTrack_.Save(j["colorTrack"]);

	std::string path = "resources/Data/UI/Animations/" + name + ".json";
	std::ofstream file(path);
	if (file.is_open()) {
		file << j.dump(4);
	}
}

UIHAnimation& UIHAnimationManager::GetAnimation(const std::string& name) {
	auto it = animations_.find(name);
	if (it == animations_.end()) {
		// ここでなにもないデータを渡す
		/*UIHAnimation ret;

		return ret;*/
	}
	return it->second;
}
void UIHAnimationManager::Load() {
	animations_.clear();
	std::string directoryPath = "resources/Data/UI/Animations/";
	// ファイルが存在するか確認
	if (!std::filesystem::exists(directoryPath))return;

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (entry.path().extension() == ".json") {
			std::ifstream file(entry.path());
			if (!file.is_open()) continue;

			json j;
			file >> j;

			// ファイル名（拡張子なし）をキーにして登録
			std::string animName = entry.path().stem().string();

			// UIHAnimationのLoadを呼び出す
			animations_[animName].Load(j);

			// JSON内のnameとファイル名が違う可能性を考慮して上書き
			animations_[animName].name = animName;
		}
	}
}