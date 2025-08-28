#include "Phase.h"
#include "GameScene.h"

void PuzzlePhase::Initialize()
{
	isEnd_ = false;
	gameScene_->SetIsPuzzleClear(false);
	gameScene_->SetIsGameClear(false);
	gameScene_->GetPlayer()->SetColor({1.0f,1.0f,1.0f,1.0f});
	gameScene_->GetPlayer()->Initialize();
	gameScene_->GetPlayer()->SetIsAlive(true);
}

void PuzzlePhase::Update()
{
	// Puzzle phase update logic
	// Transition to next phase when puzzle is solved

	puzzleTime_ += 1.0f / 60.0f;
	gameScene_->GetGround()->Update();
	gameScene_->GetPlayer()->Update();
	gameScene_->GetNazo()->Update();
	//gameScene_->GetUseCamera()->UpDate();
	for (int i = 0;i < gameScene_->GetPots().size(); i++) {
		gameScene_->GetPots().at(i)->Update();
	}
	if (InputManager::GetKey().PressedKey(DIK_0)) {
		isEnd_ = true;
	}
}

bool PuzzlePhase::IsEnd()
{
	if (!isEnd_)return false;
	gameScene_->SetIsPuzzleClear(true);
	gameScene_->SetPuzzleClearTime(puzzleTime_);
	gameScene_->ChangePhase(new BossPhase(gameScene_));
	return isEnd_;
}