#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "WorldTransform.h"
#include "Player3D.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "SpriteObject.h"
#include "Particle.h"

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
	void CollisionCheck();
private:
	std::unique_ptr<Player3D>player_;
	std::unique_ptr<BossEnemy>boss_;
	std::unique_ptr<CollisionManager>collisionManager_;
	
};



