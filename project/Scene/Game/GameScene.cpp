#include "GameScene.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"
#include "SceneDirector.h"


GameScene::GameScene()
	: player_(std::make_unique<Player3D>()),
	  boss_(std::make_unique<BossEnemy>()),
	collisionManager_(std::make_unique<CollisionManager>())
{

}

GameScene::~GameScene() {
	
	Log::ViewFile("Path GameScene ~");
}

void GameScene::Initialize() {
	player_->Initialize(p_fngine_);
	boss_->Initialize(p_fngine_,player_.get());
}

void GameScene::Update(){
	//if (player_->IsDead()) {
		//	// プレイヤーが死んだ
		//	p_sceneDirector_->RequestChangeScene(new GameOverScene);
		//	return;
		//}
		//if (boss_->IsDead()) {
		//	// ボスが死んだ
		//	p_sceneDirector_->RequestChangeScene(new ClearScene);
		//	return;
		//}

	player_->Update();

	boss_->Update();

	CollisionCheck();
}

void GameScene::Draw() {
	boss_->Draw();
	player_->Draw();
}

void GameScene::CollisionCheck() {
	// 中身をclear
	collisionManager_->Begin();

	// ここからColliderを設定
	collisionManager_->SetColliders(&player_->GetAttackCollider());
	collisionManager_->SetColliders(&player_->GetPlayerBodyCollider());
	collisionManager_->SetColliders(&boss_->GetAttackCollider());
	collisionManager_->SetColliders(&boss_->GetBossBodyCollider());
	for (auto& bullet : boss_->GetBulletManager().bullets_) {
		collisionManager_->SetColliders(&bullet->GetCollider());
	}
	

	// Check!
	collisionManager_->CheckAllCollisions();
}