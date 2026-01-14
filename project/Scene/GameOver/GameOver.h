#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "WorldTransform.h"
#include "SpriteObject.h"

class GameOverScene
	:public Scene
{
public:
	GameOverScene() {};
	~GameOverScene()override {};
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
private:
	//SpriteObject sprite_;
	std::unique_ptr<SpriteObject>sprite_;
	bool isTrans_ = false;
};
