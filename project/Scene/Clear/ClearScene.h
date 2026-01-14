#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "WorldTransform.h"
#include "SpriteObject.h"

class ClearScene
	:public Scene
{
public:
	ClearScene() {};
	~ClearScene()override {};
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
private:
	//SpriteObject sprite_;
	std::unique_ptr<SpriteObject>clearSprite_;
	bool isTrans_ = false;
};
