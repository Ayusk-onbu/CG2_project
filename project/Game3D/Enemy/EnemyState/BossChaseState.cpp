#include "EnemyState.h"
#include "Enemy.h"

// --- Initialize() ---
void BossChaseState::Initialize()
{
	if (!boss_) return;

}

// --- Update() ---
void BossChaseState::Update()
{
	if (!boss_) return;

	// 1. **プレイヤーとの位置と距離を計算**
	const Vector3& bossPos = boss_->GetPosition();
	const Vector3& targetPos = boss_->GetTargetPosition();

	if (std::isnan(targetPos.x) || std::isnan(targetPos.y) || std::isnan(targetPos.z)) {
		// プレイヤーがNaNなので、これ以上計算せず、回転処理をスキップ
		return;
	}

	// 水平方向の差分ベクトルを計算 (Y軸は無視)
	Vector3 delta = targetPos - bossPos;
	delta.y = 0.0f;

	float distance = Length(delta);

	// 2. **状態遷移ロジック**

	// --- 2-1. 攻撃範囲に入ったか？ ---
	if (distance <= MELEE_ATTACK_RANGE)
	{
		// 攻撃範囲に入ったら、意思決定 State に戻って次の行動を判断させる
		boss_->ChangeState(new BossDecisionState());
		return;
	}

	// --- 2-2. 停止距離のチェック ---
	if (distance <= MIN_DISTANCE_TO_STOP)
	{
		// 近づきすぎたら、一時的に DecisionState に戻り、他の行動（例：後退やガード）を検討させる
		// この場合、DecisionState側で後退や停止が選択されるロジックが必要
		boss_->ChangeState(new BossDecisionState());
		return;
	}


	// 3. **移動処理**

	// 進行方向ベクトルを正規化
	Vector3 direction = Normalize(delta); // Vector3::Normalize はベクトルの正規化を計算する想定

	// ボスを進行方向に回転させる処理
	boss_->LookAtTarget();

	// 移動量を計算 (移動量を BossEnemy::Move() などに設定する想定)
	Vector3 moveAmount = direction * CHASE_SPEED * (1.0f / 60.0f); // deltaTimeを使用

	// BossEnemy::Move(moveAmount); 
	boss_->SetMovement(moveAmount); // BossEnemyに移動量を設定するメソッドがある想定

	ImGuiManager::GetInstance()->Text("BossState : Chase");
}