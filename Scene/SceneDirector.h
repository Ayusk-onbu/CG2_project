#pragma once
#include "Scene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include <memory>

class SceneDirector
{
public:
	~SceneDirector();
public:
	void Initialize(Scene& firstScene);
	void ChangeScene(Scene& nextScene);
	void Run();
public:
	void SetUpFngine(Fngine& fngine) { p_fngine_ = &fngine; }
private:
	Fngine* p_fngine_;
	Scene* currentScene_;
};

