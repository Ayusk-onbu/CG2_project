#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "WorldTransform.h"
#include "Player3D.h"

class GameScene 
	:public Scene
{
public:
	GameScene();
	~GameScene()override;
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
public:
	
public:
	
public:
	
private:

private:
	std::unique_ptr<Player3D>player_;
};



