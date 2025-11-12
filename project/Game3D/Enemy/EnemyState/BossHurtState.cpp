#include "EnemyState.h"
#include "Enemy.h"

void BossHurtState::Initialize()
{
	if (!boss_) return;

	hurtTimer_ = 0.0f;

	// 2. 被ダメージ時のノックバック処理（後で実装が必要）
	// boss_->ApplyKnockback(); 

	// 3. ボスがすでに死亡していないかチェック
	if (boss_->IsDead()) {
		// 死亡状態へ遷移させる（別途 BossDeathState が必要）
		boss_->ChangeState(new BossDeathState());
		return;
	}
}

void BossHurtState::Update()
{
	if (!boss_) return;

	const float deltaTime = 1.0f / 60.0f;
	hurtTimer_ += deltaTime;

	// 1. **移動の停止**
	// のけぞり中はボスの移動を完全に停止させます
	boss_->SetMovement({ 0.0f, 0.0f, 0.0f });

	// 2. **状態遷移ロジック (終了判定)**

	// --- のけぞりアニメーション終了 ---
	if (hurtTimer_ >= maxHurtDuration_)
	{
		// のけぞりが終了したら、次の行動を判断させるために Decision State に戻る
		boss_->ChangeState(new BossDecisionState());
		return;
	}

	// --- 死亡判定（のけぞり中にHPが0になった場合） ---
	if (boss_->IsDead()) {
		// 死亡 State へ遷移
		boss_->ChangeState(new BossDeathState());
		return;
	}
	ImGuiManager::GetInstance()->Text("BossState : Hurt");
}

void BossHurtState::Exit()
{
	if (!boss_) return;

}