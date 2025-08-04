#pragma once
#include "Scene.h"
#include "GameScene.h"
#include <memory>

class SceneDirector
{
public:
	void ChangeScene(Scene& nextScene);
	void Run();
private:
	Scene* currentScene_;
};

