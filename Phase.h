#pragma once
#include "CollisionManager.h"

class GameScene;
class Player;
class Enemy;
class Phase
{
public:
	Phase(GameScene* gameScene) :gameScene_(gameScene) {}
	~Phase() = default;
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual bool IsEnd() = 0;
protected:
	GameScene* gameScene_;
	bool isEnd_ = false;
};

class TitlePhase
	:public Phase
{
public:
	TitlePhase(GameScene* gameScene) :Phase(gameScene) {}
public:
	void Initialize() override;
	void Update() override;
	bool IsEnd()override;
private:
	float titleTime_ = 0.0f;
	float titleMaxTime_ = 3.0f;
};

class PuzzlePhase
	:public Phase
{
public:
	PuzzlePhase(GameScene* gameScene) :Phase(gameScene) {}
public:
	void Initialize() override;
	void Update() override;
	bool IsEnd()override;
private:
	float puzzleTime_ = 0.0f;
	float puzzleMaxTime_ = 60.0f;
};

class RestPhase
	:public Phase
{
public:
	RestPhase(GameScene* gameScene) :Phase(gameScene) {}
public:
	void Initialize() override;
	void Update() override;
	bool IsEnd()override;
private:
	void GetPointByTime();
private:
	enum class SELECTSTATS {
		HP = 0,STR,DEF,AGI,LUCK,ALL
	};
	Player* player_ = nullptr;
	SELECTSTATS selectStats_ = SELECTSTATS::HP;
};

class BossPhase
	:public Phase
{
public:
	BossPhase(GameScene* gameScene) :Phase(gameScene) {}
public:
	void Initialize() override;
	void Update() override;
	bool IsEnd()override;
private:
	void Failed();
private:
	Player* player_ = nullptr;
	Enemy* boss_ = nullptr;
	CollisionManager collision_;

	float battleTime_ = 0.0f;
	float battleMaxTime_ = 15.0f;
};

