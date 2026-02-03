#pragma once
#include "UIElement.h"
#include <list>

class UIContainer 
	:public UIElement
{
public:
	UIContainer();
public:
	void UpdateAnimation(float deltaTime);
	void Play(const std::unordered_map<std::string, std::string>&animMap_,bool isLoop = false);
	void Draw()override;
	void Load(const json& j)override;
	void LoadbyFile(const std::string& name);
	void Save(json& j)const override;
	void SavetoFile();
	void DrawImGui()override;
	bool AllEnded()const;

	void AddElement();
	void AddContainer();
private:
	// [ 子供 ]
	std::list<std::unique_ptr<UIElement>> elements_;
	// [ 描画順ソート用 ]
	std::vector<UIElement*> sortedElements_;
	// [ 選択されている要素 ]
	UIElement* currentFocus_;
	std::string inputName_;
	bool isAllPlaying_ = false;
};