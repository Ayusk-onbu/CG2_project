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
	SpriteObject sprite_;
	WorldTransform world_;
	int textureHandle_ = -1;
	bool isTrans_ = false;
};
