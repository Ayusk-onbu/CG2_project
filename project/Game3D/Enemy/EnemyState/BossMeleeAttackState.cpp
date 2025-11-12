#include "EnemyState.h"
#include "Enemy.h"

void BossMeleeAttackState::Initialize()
{
	if (!boss_) return;

	attackTimer_ = 0.0f;

	boss_->SetIsViewAttack(true);

	// ボスをプレイヤーの方向に向かせる（攻撃を外さないように）
	//boss_->LookAtTarget(); // BossEnemyに回転処理メソッドが必要
}

void BossMeleeAttackState::Update()
{
	if (!boss_) return;

	const float deltaTime = 1.0f / 60.0f;
	attackTimer_ += deltaTime;

	// 1. **攻撃判定の制御**

	// --- 攻撃判定の有効期間 ---
	if (attackTimer_ >= hitBoxActiveStart_ && attackTimer_ <= hitBoxActiveEnd_)
	{
		// 攻撃判定を有効化する

		// プレイヤーの位置を基準に、ボスの前方に攻撃判定を配置
		Vector3 bossPos = boss_->GetPosition();

		// ボスの前方ベクトルを取得 (BossEnemyに実装が必要)
		Vector3 forward = boss_->GetForwardVector();

		// ボスから一定距離離れた位置を攻撃判定の中心とする
		Vector3 hitBoxWorldPos = bossPos + (forward * attackRange_);

		// 💡 攻撃判定を有効化し、位置と半径を更新
		boss_->EnableAttackHitBox(true, hitBoxWorldPos, attackRadius_);
	}
	else
	{
		// 攻撃判定を無効化
		boss_->EnableAttackHitBox(false, boss_->GetPosition(), 0.0f);
	}


	// 2. **状態遷移ロジック (終了判定)**

	// --- 攻撃アニメーション終了 ---
	if (attackTimer_ >= maxAttackDuration_)
	{
		// 攻撃が終了したら、再び意思決定 State に戻る
		boss_->ChangeState(new BossDecisionState());
		return;
	}

	// 3. **被ダメージチェック (中断判定)**
	/*
	// ボスが攻撃中にダメージを受けてのけぞり状態に移行する場合
	if (boss_->IsHurtByAttack()) {
		boss_->ChangeState(new BossHurtState());
		return;
	}
	*/

	ImGuiManager::GetInstance()->Text("BossState : MeleeAttack");
}

void BossMeleeAttackState::Exit()
{
	if (!boss_) return;

	// 攻撃判定を確実に無効化
	boss_->EnableAttackHitBox(false, boss_->GetPosition(), 0.0f);

	boss_->SetIsViewAttack(false);

	// 攻撃終了後のクールダウンタイマーを設定
	// boss_->SetCooldown(ATTACK_MELEE, 2.0f); 
}