#pragma once
#include "Scene.h"
#include "GameScene.h"
#include "TestScene.h"
#include <memory>

class SceneDirector
{
public:
	~SceneDirector();
public:
	void Initialize(Scene& firstScene);
	void Run();
	void RequestChangeScene(Scene* newScene);
public:
	void SetUpFngine(Fngine& fngine) { p_fngine_ = &fngine; }
private:
	Fngine* p_fngine_;
	Scene* currentScene_;
};

