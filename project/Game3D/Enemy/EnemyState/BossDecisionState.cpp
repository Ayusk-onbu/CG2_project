#include "Enemy.h"

void BossDecisionState::Initialize()
{
	if (!boss_) return;

	// 1. **現在の状況を取得**
	const Vector3& bossPos = boss_->GetPosition(); // BossEnemyにGetPosition()が必要
	const Vector3& targetPos = boss_->GetTargetPosition();

	if (std::isnan(targetPos.x) || std::isnan(targetPos.y) || std::isnan(targetPos.z)) {
		// プレイヤーがNaNなので、これ以上計算せず、回転処理をスキップ
		return;
	}

	// プレイヤーとの水平距離（Y軸は無視）を計算
	Vector3 delta = targetPos - bossPos;
	delta.y = 0.0f;
	float distance = Length(delta); // Vector3::Lengthはベクトルの長さを計算する想定

	// 2. **フェーズに応じた特別な処理のチェック**
	/* // ボスがHPの閾値を下回ったらフェーズ変更 State に遷移（例: HPが30%未満）
	if (boss_->GetHP() < boss_->GetMaxHP() * RETREAT_HP_THRESHOLD) {
		// boss_->ChangeState(new BossPhaseChangeState());
		// return;
	}
	*/

	// 3. **行動の評価と優先順位付け**

	// --- 3-1. 近接攻撃の範囲内か？ ---
	if (distance <= MELEE_RANGE)
	{
		// プレイヤーが近すぎる場合、近接攻撃を優先
		// boss_->IsCooldownReady(ATTACK_MELEE) などのクールダウンチェックが必要
		boss_->ChangeState(new BossMeleeAttackState());
		return;
	}

	// --- 3-2. 遠距離攻撃の範囲内か？ ---
	if (distance >= RANGE_ATTACK_MIN_DIST)
	{
		// 遠距離攻撃を考慮する距離の場合
		// 確率（例：40%）で遠距離攻撃 State へ遷移
		 if (rand() % 10 < 4) { 
		 	boss_->ChangeState(new BossRangeAttackState());
		 	return;
		 }
	}

	// 4. **デフォルトの行動：追跡**
	// どの攻撃条件も満たさない場合、またはクールダウン中の場合は、プレイヤーに近づく
	boss_->ChangeState(new BossChaseState());
}

// DecisionStateは基本的に遷移するだけで、 Update()は実行されない想定ですが、
// 念のため定義しておきます。
void BossDecisionState::Update()
{
	// DecisionStateはInitializeで遷移を完了するため、通常ここには来ません。
	ImGuiManager::GetInstance()->Text("BossState : Decision");
}