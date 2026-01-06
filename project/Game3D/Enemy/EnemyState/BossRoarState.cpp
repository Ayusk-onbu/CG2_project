#include "EnemyState.h"
#include "Enemy.h"

void BossRoarState::Initialize() {

}

void BossRoarState::Update() {
	boss_->ChangeState(new BossDecisionState());
}

void BossRoarState::Exit() {

}