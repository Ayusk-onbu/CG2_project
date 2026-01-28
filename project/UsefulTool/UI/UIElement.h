#pragma once
#include "SpriteObject.h"
#include "UIHAnimation.h"
#include <json.hpp>
using json = nlohmann::json;

class UIContainer;

class UIElement {
public:
	virtual ~UIElement() = default;
public:
	virtual void Initialize(Fngine* engine);
	// 出てくるときの処理
	virtual void UpdateFirst() {}
	// 出ている際の処理
	virtual void UpdateMiddle() {}
	// 消える際の処理
	virtual void UpdateFinalize() {}

	virtual void Draw();

	virtual void DrawImGui();

	void DrawImGuiElement();

	virtual void Save(json& j)const;

	virtual void Load(const json& j);

	SpriteObject& GetObj() { return *obj_.get(); }
public:
	void PlayAnimation(const std::string& name, bool loop = false);
	void UpdateAnimation(float deltaTime);
	void SetParent(UIContainer* other) { parent_ = other; }
private:
	void ApplyTransform();
public:
	// Engine
	Fngine* engine_;
	// 親の位置
	UIContainer* parent_ = nullptr;
	Matrix4x4 parentMat_;
	// 名前
	std::string name_;
	// 表示、非表示のフラグ
	bool isActive_ = true;
	bool isVisible_ = true;
	// Texture
	std::string selectTexture_;
	// スプライト本人
	std::unique_ptr<SpriteObject>obj_;

	struct PlayState {
		bool isPlaying = false;
		float currentTime = 0.0f;
		float duration = 0.0f;
		bool isLoop = false;
		std::string currentAnimName;
	};

	PlayState playState_;

	// 将来的に使えたらいいよね一覧
	// 親と共有するため
	float finalAlpha_ = 1.0f;
	// 画面のどこか
	Vector2 anchor_ = { 0.0f,0.0f };
	// 自身の中心はどこか
	Vector2 pivot_ = { 0.5f, 0.5f };

	Vector3 relativePos_;
	// 自身のNumber
	int id_;
};