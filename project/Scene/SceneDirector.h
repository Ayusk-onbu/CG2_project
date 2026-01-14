#pragma once
#include "Scene.h"
#include "GameScene.h"
#include "TestScene.h"
#include "GameOver.h"
#include "ClearScene.h"
#include <memory>

class SceneDirector
{
public:
	~SceneDirector();
public:
	void Initialize(Scene& firstScene);
	void Update();
	void Draw();
	void ImGui();
	void RequestChangeScene(Scene* newScene);
public:
	void SetUpFngine(Fngine& fngine) { p_fngine_ = &fngine; }
private:
	void LoadModelData();
	void LoadTexture();
	void LoadMusic();
private:
	Fngine* p_fngine_;
	Scene* currentScene_;
	bool isGameRunning_ = true;
};

