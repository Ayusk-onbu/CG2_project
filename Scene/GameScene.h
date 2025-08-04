#pragma once
#include "Scene.h"
#include "Player.h"

class GameScene 
	:public Scene
{
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
public:// ゲームにまつわる関数
private:
	std::unique_ptr<Player> player_;
};

