#include "Phase.h"
#include "GameScene.h"

void RestPhase::Initialize()
{
	isEnd_ = false;
	player_ = gameScene_->GetPlayer();
}

void RestPhase::Update()
{
	// Rest phase update logic
	// Transition to next phase after a certain condition or time
	float point = gameScene_->GetPoint();
	float hp = player_->GetStats().GetHp();
	float str = player_->GetStats().GetStrength();
	float def = player_->GetStats().GetDefense();
	float agi = player_->GetStats().GetAgility();
	float luck = player_->GetStats().GetLuck();
	if (InputManager::GetKey().PressedKey(DIK_UP)) {
		switch (selectStats_) {
		case SELECTSTATS::HP:
			selectStats_ = SELECTSTATS::LUCK;
			break;
		case SELECTSTATS::STR:
			selectStats_ = SELECTSTATS::HP;
			break;
		case SELECTSTATS::DEF:
			selectStats_ = SELECTSTATS::STR;
			break;
		case SELECTSTATS::AGI:
			selectStats_ = SELECTSTATS::DEF;
			break;
		case SELECTSTATS::LUCK:
			selectStats_ = SELECTSTATS::AGI;
			break;
		}
	}
	if (InputManager::GetKey().PressedKey(DIK_DOWN)) {
		switch (selectStats_) {
		case SELECTSTATS::HP:
			selectStats_ = SELECTSTATS::STR;
			break;
		case SELECTSTATS::STR:
			selectStats_ = SELECTSTATS::DEF;
			break;
		case SELECTSTATS::DEF:
			selectStats_ = SELECTSTATS::AGI;
			break;
		case SELECTSTATS::AGI:
			selectStats_ = SELECTSTATS::LUCK;
			break;
		case SELECTSTATS::LUCK:
			selectStats_ = SELECTSTATS::HP;
			break;
		}
	}
	if (InputManager::GetKey().PressedKey(DIK_RIGHT) && point > 0) {
		switch (selectStats_) {
		case SELECTSTATS::HP:
			player_->GetStats().SetHp(hp + 1);
			break;
		case SELECTSTATS::STR:
			player_->GetStats().SetStrength(str + 1);
			break;
		case SELECTSTATS::DEF:
			player_->GetStats().SetDefense(def+ 1);
			break;
		case SELECTSTATS::AGI:
			player_->GetStats().SetAgility(agi + 1);
			break;
		case SELECTSTATS::LUCK:
			player_->GetStats().SetLuck(luck + 1);
			break;
		}
		point -= 1.0f;
	}
	if (InputManager::GetKey().PressedKey(DIK_LEFT)) {
		switch (selectStats_) {
		case SELECTSTATS::HP:
			player_->GetStats().SetHp(hp - 1);
			break;
		case SELECTSTATS::STR:
			player_->GetStats().SetStrength(str - 1);
			break;
		case SELECTSTATS::DEF:
			player_->GetStats().SetDefense(def - 1);
			break;
		case SELECTSTATS::AGI:
			player_->GetStats().SetAgility(agi - 1);
			break;
		case SELECTSTATS::LUCK:
			player_->GetStats().SetLuck(luck - 1);
			break;
		}
		point += 1.0f;
	}
	gameScene_->SetPoint(point);
	/*ImGui::Begin("Rest Phase");
	ImGui::Text("Point: %f", point);
	ImGui::Text("Select Stats:");
	ImGui::Text("HP: %f %s", player_->GetStats().GetHp(), (selectStats_ == SELECTSTATS::HP) ? "<--" : "");
	ImGui::Text("STR: %f %s", player_->GetStats().GetStrength(), (selectStats_ == SELECTSTATS::STR) ? "<--" : "");
	ImGui::Text("DEF: %f %s", player_->GetStats().GetDefense(), (selectStats_ == SELECTSTATS::DEF) ? "<--" : "");
	ImGui::Text("AGI: %f %s", player_->GetStats().GetAgility(), (selectStats_ == SELECTSTATS::AGI) ? "<--" : "");
	ImGui::Text("LUCK: %f %s", player_->GetStats().GetLuck(), (selectStats_ == SELECTSTATS::LUCK) ? "<--" : "");
	ImGui::End();*/
	if (InputManager::GetKey().PressedKey(DIK_RETURN)) {
		isEnd_ = true;
	}
}

bool RestPhase::IsEnd()
{
	if (!isEnd_) return false;

	gameScene_->ChangePhase(new BossPhase(gameScene_));

	return true;
}

void RestPhase::GetPointByTime()
{
	float time = gameScene_->GetPuzzleClearTime();
	float point = 0.0f;
	if (time <= 20.0f) {
		point = 5.0f;
	}
	else if (time <= 40.0f) {
		point = 3.0f;
	}
	else {
		point = 1.0f;
	}
	gameScene_->SetPoint(point);
}