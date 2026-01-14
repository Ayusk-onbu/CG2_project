#include "EnemyState.h"
#include "Enemy.h"

void BossRangeAttackState::Initialize()
{
	if (!boss_) return;

	attackTimer_ = 0.0f;
	hasShot_ = false;

	// ボスをプレイヤーの方向に向かせる（詠唱中にプレイヤーが動いても攻撃が当たるように）
	boss_->LookAtTarget();
}

void BossRangeAttackState::Update()
{
	if (!boss_) return;

	const float deltaTime = 1.0f / 60.0f;
	attackTimer_ += deltaTime;

	// 飛び道具生成に必要な情報を計算
	Vector3 bossPos = boss_->GetPosition();
	Vector3 targetPos = boss_->GetTargetPosition();

	if (std::isnan(targetPos.x) || std::isnan(targetPos.y) || std::isnan(targetPos.z)) {
		// プレイヤーがNaNなので、これ以上計算せず、回転処理をスキップ
		return;
	}

	// 飛び道具の発射位置を、ボスの位置から少し上に設定
	Vector3 startPos = { bossPos.x, bossPos.y + 1.5f, bossPos.z };

	// プレイヤーの方向へ発射するための方向ベクトルを設定
	Vector3 direction = Normalize(targetPos - startPos);

	// 1. **飛び道具の生成**
	// 指定されたタイミングで一度だけ実行
	if (attackTimer_ >= projectileShootTime_ && !hasShot_)
	{
		// スピードを設定
		float sideSpeed = 2.5f;
		// 角度を設定
		float angleOffset = Deg2Rad(30);
		// 発射処理
		// [ 右側（30度回転 / 速度：速め）]
		Matrix4x4 matRotRight = Matrix4x4::Make::RotateY(angleOffset);
		Vector3 velocityRight = Matrix4x4::Transform(direction, matRotRight) * sideSpeed;
		boss_->GetBulletManager().CreateBullet(startPos, velocityRight, COL_Enemy_Attack, COL_Player);

		// [ 左側（-30度回転 / 速度：速め）]
		Matrix4x4 matRotLeft = Matrix4x4::Make::RotateY(-angleOffset);
		Vector3 velocityLeft = Matrix4x4::Transform(direction, matRotLeft) * sideSpeed;
		boss_->GetBulletManager().CreateBullet(startPos, velocityLeft, COL_Enemy_Attack, COL_Player);

		hasShot_ = true; // 生成フラグを立てて、二重発射を防ぐ
	}


	// 2. **状態遷移ロジック (終了判定)**

	// --- 終了 ---
	if (attackTimer_ >= maxAttackDuration_)
	{
		// [ 真ん中 ]
		float speedCenter = 1.0f;
		Vector3 velocityCenter = direction * speedCenter;
		// 飛び道具の生成処理
		boss_->GetBulletManager().CreateBullet(
			startPos,
			velocityCenter,
			COL_Enemy_Attack, // 敵の攻撃属性
			COL_Player        // プレイヤーに当たるように
		);
		// 攻撃が終了したら、再び意思決定 State に戻る
		boss_->ChangeState(new BossDecisionState());
		return;
	}

	// 3. **被ダメージチェック (中断判定)**
	/*
	// ボスが詠唱中にダメージを受けてのけぞり状態に移行する場合
	if (boss_->IsHurtByAttack()) {
		boss_->ChangeState(new BossHurtState());
		return;
	}
	*/
	ImGuiManager::GetInstance()->Text("BossState : RangeAttack");
}

void BossRangeAttackState::Exit()
{
	if (!boss_) return;

	// 遠距離攻撃終了後のクールダウンタイマーを設定
	// boss_->SetCooldown(ATTACK_RANGE, 4.0f); // 遠距離攻撃はクールダウン長め
}