#include "Phase.h"
#include "GameScene.h"

void BossPhase::Initialize()
{
	player_ = gameScene_->GetPlayer();
	boss_ = gameScene_->GetEnemy();
	gameScene_->SetIsEnemyView(true);
	isEnd_ = false;
	gameScene_->SetIsGameClear(false);
	gameScene_->SetIsFailed(false);
}

void BossPhase::Update()
{
	// Boss phase update logic
	// Transition to next phase when boss is defeated

	battleTime_ += 1.0f / 60.0f;

	gameScene_->GetT()->Update(battleTime_ / battleMaxTime_);

	collision_.Begin();
	player_->Update();
	boss_->Update(*player_);
	player_->SetEnemy(boss_);

	collision_.SetColliders(player_);
	collision_.SetColliders(boss_);
	if (battleTime_ < battleMaxTime_) {
		collision_.CheckAllCollisions();
	}
	if (player_->IsAlive()) {
		if (battleTime_ >= battleMaxTime_) {
			gameScene_->SetIsGameClear(true);
			gameScene_->GetFailedClear()->SetIsClear(true);
			if (InputManager::GetKey().PressedKey(DIK_SPACE))
				isEnd_ = true;
		}
	}
	Failed();
}

bool BossPhase::IsEnd()
{
	if (!isEnd_)return false;
	gameScene_->SetIsEnemyView(false);
	gameScene_->ChangePhase(new TitlePhase(gameScene_));
	return isEnd_;
}

void BossPhase::Failed()
{
	if (!player_->IsAlive()) {
		gameScene_->SetIsFailed(true);
		gameScene_->GetFailedClear()->SetIsClear(false);
		// Game Over処理
		if(InputManager::GetKey().PressedKey(DIK_SPACE))
		isEnd_ = true;
	}
}