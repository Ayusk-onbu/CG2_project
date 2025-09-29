#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "Cameras.h"

class GameScene 
	:public Scene
{
public:
	GameScene()=default;
	~GameScene();
public:// Sceneにまつわる関数
	void Initialize()override;
	void Update()override;
	void Draw()override;
public:
	
public:
	
public:// ゲームにまつわる関数
	
private:
	// カメラ
	CameraBase* useCamera_;
	// 第三者視点
	std::unique_ptr<Camera>mainCamera_;
	// 一人称視点
	std::unique_ptr<Camera>fpsCamera_;
};



