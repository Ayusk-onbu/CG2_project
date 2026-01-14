#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "WorldTransform.h"
#include "Player3D.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "SpriteObject.h"
#include "Particle.h"
#include "Ground.h"

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
private:
	void CollisionCheck();
private:
	void ToScene();
	void ToClearScene();
	void ToGameOverScene();
	float toSceneTimer_ = 0.0f;
private:
	// Fade関係
	void FirstFade(float time);
	std::unique_ptr<SpriteObject>fadeUp_;
	std::unique_ptr<SpriteObject>fadeDown_;
	float toGameTimer_ = 0.0f;
	bool notGame_ = true;
private:
	// UI関係
	std::unique_ptr<SpriteObject>playUI_;
	std::unique_ptr<SpriteObject>purposeUI_;
private:

	std::unique_ptr<Player3D>player_;
	std::unique_ptr<BossEnemy>boss_;
	std::unique_ptr<CollisionManager>collisionManager_;
	std::unique_ptr<Ground>ground_;
};



