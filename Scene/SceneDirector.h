#pragma once
#include "Scene.h"
#include "GameScene.h"
#include <memory>

class SceneDirector
{
public:
	/// <summary>
	/// シーン切り替え
	/// </summary>
	void ChangeScene(std::unique_ptr<Scene> nextScene);
	void Run();
private:
	std::unique_ptr<Scene> currentScene_;
};

