#include "Phase.h"
#include "GameScene.h"

void TitlePhase::Initialize()
{
	isEnd_ = false;
	gameScene_->SetIsPuzzleClear(false);
	gameScene_->SetIsGameClear(false);
	gameScene_->GetTitle()->Initialize();
	gameScene_->GetPlayer()->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	titleTime_ = 0.0f;

	gameScene_->SetIsFailed(false);
	gameScene_->SetIsViewCF(false);
	gameScene_->GetFailedClear()->SetIsClear(false);
}

void TitlePhase::Update()
{
	// Title phase update logic
	// Transition to next phase when start button is pressed
	gameScene_->SetIsViewCF(false);
	gameScene_->GetTitle()->Update();

	if (InputManager::GetKey().PressedKey(DIK_SPACE)) {
		isEnd_ = true;
	}
	/*ImGui::Begin("Game");
	ImGui::Text("Title");
	ImGui::End();*/
}

bool TitlePhase::IsEnd()
{
	if (!isEnd_)return false;
	titleTime_ += 1.0f / 60.0f;
	gameScene_->GetTitle()->Move(titleTime_ / titleMaxTime_);
	if (titleTime_ > titleMaxTime_) {
		gameScene_->ChangePhase(new PuzzlePhase(gameScene_));
	}
	return isEnd_;
}