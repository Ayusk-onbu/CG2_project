#pragma once
#include "Fngine.h"
#include "Scene.h"
#include "Cameras.h"

#include "Phase.h"

#include "Title.h"
#include "Ground.h"
#include "Wall.h"
#include "Player.h"
#include "Enemy.h"
#include "Pot.h"
#include "FailedClear.h"
#include "T.h"
#include "LS.h"
#include "Nazo.h"

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
	void ChangePhase(Phase* newPhase);
public:
	// Get
	Player* GetPlayer() { return player_.get(); }
	Enemy* GetEnemy() { return enemy_.get(); }
	Title* GetTitle() { return title_.get(); }
	T* GetT() { return t_.get(); }
	Nazo* GetNazo() { return nazo_.get(); }
	FailedClear* GetFailedClear() { return failedClear_.get(); }
	CameraBase* GetUseCamera() { return useCamera_; }
	CameraBase* GetMainCamera() { return mainCamera_.get(); }
	CameraBase* GetFPSCamera() { return fpsCamera_.get(); }
	std::vector<std::unique_ptr<Pot>>& GetPots() { return pots_; }
	std::unique_ptr<Ground>& GetGround() { return ground_; }
	float GetPoint() { return point_; }
	bool GetIsPuzzleClear() { return isPuzzleClear_; }
	float GetPuzzleClearTime() { return puzzleClearTime_; }
	bool GetIsGameClear() { return isGameClear_; }
	bool GetIsFailed() { return isFailed_; }
public:// ゲームにまつわる関数
	void SetCamera(CameraBase& camera);
	void SetPoint(float point) { point_ = point; }
	void SetIsPuzzleClear(bool isPuzzleClear) { isPuzzleClear_ = isPuzzleClear; }
	void SetPuzzleClearTime(float puzzleClearTime) { puzzleClearTime_ = puzzleClearTime; }
	void SetIsGameClear(bool isGameClear) { isGameClear_ = isGameClear; }
	void SetIsFailed(bool isFailed) { isFailed_ = isFailed; }
	void SetIsViewCF(bool isViewCF) { isViewCF_ = isViewCF; }
	bool SetIsEnemyView(bool isEnemyView) { return isEnemyView_ = isEnemyView; }
private:
	Phase* currentPhase_;
	// ポイント
	float point_ = 0;
	// パズルクリアフラグ
	bool isPuzzleClear_ = false;
	// パズルクリアタイム
	float puzzleClearTime_ = 0.0f;
	// ゲームのクリアフラグ
	bool isGameClear_ = false;
	bool isFailed_ = false;
	bool isViewCF_ = false;
	// タイトル
	std::unique_ptr<Title> title_;
	std::unique_ptr<ModelObject>titleModel_;
	std::unique_ptr<Texture>titleTexture_;
	// 地面
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<ModelObject>groundModel_;
	std::unique_ptr<Texture>groundTexture_;
	// 壁
	std::unique_ptr<Wall> wall_;
	std::unique_ptr<ModelObject>wallModel_;
	std::unique_ptr<Texture>wallTexture_;
	// プレイヤー
	std::unique_ptr<Player> player_;
	std::unique_ptr<ModelObject>playerModel_;
	std::unique_ptr<Texture>playerTexture_;
	// 敵
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<ModelObject>enemyModel_;
	std::unique_ptr<Texture>enemyTexture_;
	bool isEnemyView_ = false;
	// 壺
	std::vector<std::unique_ptr<Pot>> pots_;
	std::unique_ptr<ModelObject>potModel_;
	std::unique_ptr<Texture>potTexture_;
	uint32_t potCount_ = 3;
	// 失敗・クリア画面
	std::unique_ptr<FailedClear> failedClear_;
	
	std::unique_ptr<ModelObject>failedModel_;
	std::unique_ptr<Texture>failedTexture_;
	std::unique_ptr<ModelObject>clearModel_;
	std::unique_ptr<Texture>clearTexture_;
	// T
	std::unique_ptr<T> t_;
	std::unique_ptr<ModelObject>tModel_;
	// LS
	std::unique_ptr<LS> ls_;
	std::unique_ptr<ModelObject>lsModel_;
	// 謎
	std::unique_ptr<Nazo> nazo_;
	// 謎モデル
	std::unique_ptr<ModelObject>nazoModel_;

	// カメラ
	CameraBase* useCamera_;
	// 第三者視点
	std::unique_ptr<Camera>mainCamera_;
	// 一人称視点
	std::unique_ptr<Camera>fpsCamera_;
};



