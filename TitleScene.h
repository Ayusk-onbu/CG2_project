#pragma once
#include "Scene.h"

class TitleScene
	: public Scene
{
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
	const char* NextSceneRequest()override;
public:// ゲームにまつわる関数

private:

};

