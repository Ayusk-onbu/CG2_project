#pragma once
#include "Structures.h"
#include "Easing.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <json.hpp>
using json = nlohmann::json;

class UIElement;

template<typename tValue>
struct UIHKeyframe {
	// [ 時間 ]
	float time;
	// [ 値 ]
	tValue value;
	// [ 次のキーへの補間方法 ]
	EASINGTYPE easeType = EASINGTYPE::None;

	void Save(json& j)const {
		j["time"] = time;
		j["easeType"] = (int)easeType;
		if constexpr (std::is_same_v<tValue, Vector4>) {
			j["value"] = { value.x, value.y, value.z, value.w };
		}
		else if constexpr (std::is_same_v<tValue, Vector3>) {
			j["value"] = { value.x, value.y, value.z };
		}
		else if constexpr (std::is_same_v<tValue, Vector2>) {
			j["value"] = { value.x, value.y };
		}
		else if constexpr (std::is_same_v<tValue, float>) {
			j["value"] = value;
		}
	}

	void Load(const json& j) {
		time = j.at("time").get<float>();
		easeType = (EASINGTYPE)j.at("easeType").get<int>();

		auto& valueJson = j.at("value");
		if constexpr (std::is_same_v<tValue, Vector4>) {
			value.x = valueJson[0];
			value.y = valueJson[1];
			value.z = valueJson[2];
			value.w = valueJson[3];
		}
		else if constexpr (std::is_same_v<tValue, Vector3>) {
			value.x = valueJson[0];
			value.y = valueJson[1];
			value.z = valueJson[2];
		}
		else if constexpr (std::is_same_v<tValue, Vector2>) {
			value.x = valueJson[0];
			value.y = valueJson[1];
		}
		else {
			value = valueJson;
		}
	}
};

template<typename tValue>
class UIHAnimationTrack {
	// [ キーフレームのリスト ]
	std::vector<UIHKeyframe<tValue>> keyframes_;
public:
	// エディタ
	void SetKeyframe(float time, const tValue& value, EASINGTYPE ease = EASINGTYPE::None) {
		// 1. すでに同じ時間のキーフレームがあるか探す
		auto it = std::find_if(keyframes_.begin(), keyframes_.end(), [time](const auto& k) {
			return std::abs(k.time - time) < 0.001f; // 微小な差は同じ時間とみなす
		});

		if (it != keyframes_.end()) {
			// 上書き
			it->value = value;
			it->easeType = ease;
		}
		else {
			// 新規追加
			keyframes_.push_back({ time, value, ease });
			// 追加した後に時間順でソート
			std::sort(keyframes_.begin(), keyframes_.end(), [](const auto& a, const auto& b) {
				return a.time < b.time;
			});
		}
	}

	tValue Evaluate(float currentTime) {
		// 要素が存在するか確認する
		if (keyframes_.empty())return tValue{};
		// 要素が複数あるのか確認する
		if (keyframes_.size() == 1)return keyframes_[0].value;

		// 時間が範囲外なのかどうか確認する
		if (currentTime <= keyframes_.front().time)return keyframes_.front().value;
		if (currentTime >= keyframes_.back().time) return keyframes_.back().value;

		// 時刻に値するキーフレームを探す
		for (size_t i = 0; i < keyframes_.size() - 1; ++i) {
			// [ 使う値を取得 ]
			auto& start = keyframes_[i];
			auto& end = keyframes_[i + 1];

			// 時間が範囲内か確認する
			if ((currentTime >= start.time) && (currentTime <= end.time)) {
				float localNow = currentTime - start.time;
				float localEnd = end.time - start.time;

				return Easing(start.value, end.value, localNow, localEnd, start.easeType);
			}
		}
		return keyframes_.back().value;
	}

public:
	float GetMaxTime()const {
		return keyframes_.empty() ? 0.0f : keyframes_.back().time;
	}

	bool IsEmpty()const {
		return keyframes_.empty();
	}

	// 特定の時間のキーフレームを削除する（エディタ用）
	void RemoveKeyframe(float time) {
		keyframes_.erase(std::remove_if(keyframes_.begin(), keyframes_.end(), [time](const auto& k) {
			return std::abs(k.time - time) < 0.01f;
			}), keyframes_.end());
	}

	// 全削除
	void Clear() { keyframes_.clear(); }

	// 現在のキーフレームリストを取得（エディタの描画用）
	const std::vector<UIHKeyframe<tValue>>& GetKeyframes() const { return keyframes_; }

	void Save(json& j) const {
		j = json::array();
		for (const auto& key : keyframes_) {
			json keyJson;
			key.Save(keyJson);
			j.push_back(keyJson);
		}
	}

	void Load(const json& j) {
		keyframes_.clear();
		if (!j.is_array()) return;
		for (const auto& keyJson : j) {
			UIHKeyframe<tValue> key;
			key.Load(keyJson);
			keyframes_.push_back(key);
		}
		// ロード後に時間順にソート（念のため）
		std::sort(keyframes_.begin(), keyframes_.end(), [](const auto& a, const auto& b) {
			return a.time < b.time;
			});
	}
};

class UIHAnimation {
public:
	// Animationの名前
	std::string name;
	// ①[ 操作したい値一覧 ]->②[ EditorFunction ]
	UIHAnimationTrack<Vector3> posTrack_;
	UIHAnimationTrack<Vector3> scaleTrack_;
	UIHAnimationTrack<Vector3> rotateTrack_;
	UIHAnimationTrack<Vector4> colorTrack_;
public:
#pragma region EditorFunctions
	// ②[ プロパティを指定してキーを打つ ]->③[ Load ]
	void SetPositionKey(float time, Vector3 pos, EASINGTYPE ease) { posTrack_.SetKeyframe(time, pos, ease); }
	void SetScaleKey(float time, Vector3 scale, EASINGTYPE ease) { scaleTrack_.SetKeyframe(time, scale, ease); }
	void SetRotateKey(float time, Vector3 rotate, EASINGTYPE ease) { rotateTrack_.SetKeyframe(time, rotate, ease); }
	void SetColorKey(float time, Vector4 color, EASINGTYPE ease) { colorTrack_.SetKeyframe(time, color, ease); }
#pragma endregion
public:
	// Load
	void Load(const json& j);
	// Save
	void Save();
	void Apply(UIElement* element, float currentTime);
	float GetDuration()const {
		return (std::max)({ posTrack_.GetMaxTime(), scaleTrack_.GetMaxTime(), colorTrack_.GetMaxTime(), rotateTrack_.GetMaxTime()});
	}
};

class UIHAnimationManager {
public:
	static UIHAnimationManager* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<UIHAnimationManager>();
		}
		return instance_.get();
	}
	void Load();
public:
	UIHAnimation& GetAnimation(const std::string& name);
	// 登録されている全アニメーションの名前リストを返す
	std::vector<std::string> GetAnimationNames() const {
		std::vector<std::string> names;
		names.reserve(animations_.size());
		for (const auto& [name, anim] : animations_) {
			names.push_back(name);
		}
		return names;
	}

	// 編集用にMap自体への参照も持っておくと便利な場合があります
	const std::unordered_map<std::string, UIHAnimation>& GetAnimations() const {
		return animations_;
	}
private:
	static std::unique_ptr<UIHAnimationManager>instance_;
	std::unordered_map<std::string, UIHAnimation>animations_;

};