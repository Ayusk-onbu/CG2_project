#pragma once

class BossEnemy; // 前方宣言

// ボス用 State 抽象クラス
class BossState
{
public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
	void SetBoss(BossEnemy* boss) { boss_ = boss; }
protected:
	BossEnemy* boss_ = nullptr;
};

// ** BossStates.h にて定義 **

// 1. 待機・意思決定を行う State
class BossDecisionState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override {} // 何も処理しない
private:
	// 行動の評価基準となる定数
	float MELEE_RANGE = 4.0f; // 近接攻撃の有効距離
	float RANGE_ATTACK_MIN_DIST = 8.0f; // 遠距離攻撃を始める最低距離
	float RETREAT_HP_THRESHOLD = 0.3f; // HPが30%未満の場合に後退を考慮
};

// 2. プレイヤーに近づく State
class BossChaseState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override {}
private:
	float CHASE_SPEED = 5.0f;          // ボスの移動速度
	float MELEE_ATTACK_RANGE = 4.0f;   // プレイヤーとの距離がこれ以下になったら攻撃を考慮
	float MIN_DISTANCE_TO_STOP = 2.0f; // プレイヤーに近づきすぎたら一時停止を考慮する距離
};

// 3. 近接攻撃の State
class BossMeleeAttackState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override;
private:
	float attackTimer_ = 0.0f;
	const float maxAttackDuration_ = 1.0f; // 攻撃全体の持続時間

	// 攻撃判定が有効になる期間
	const float hitBoxActiveStart_ = 0.3f; // 攻撃開始から 0.3秒後
	const float hitBoxActiveEnd_ = 0.5f;   // 攻撃開始から 0.5秒後

	const float attackRange_ = 2.5f; // 攻撃判定の届く距離
	const float attackRadius_ = 1.0f; // 攻撃判定の半径
};

// 4. ダメージを受けた際の State
class BossHurtState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override;
private:
	float hurtTimer_ = 0.0f;          // のけぞりの経過時間
	const float maxHurtDuration_ = 0.5f; // のけぞり状態の持続時間（0.5秒）
};

// 5. 遠距離攻撃の State
class BossRangeAttackState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override;
private:
	float attackTimer_ = 0.0f;          // 攻撃の経過時間
	const float maxAttackDuration_ = 1.5f; // 攻撃全体の持続時間 (長めの詠唱時間などを想定)

	// 飛び道具を生成するタイミング
	const float projectileShootTime_ = 0.8f; // 攻撃開始から 0.8秒後
	bool hasShot_ = false; // 飛び道具を一度だけ生成するためのフラグ
};

// 6. 死亡の State
class BossDeathState
	:public BossState
{
public:
	void Initialize() override;
	void Update() override;
	void Exit() override {}
private:
	float deathTimer_ = 0.0f;           // 演出の経過時間
	const float maxDeathDuration_ = 3.0f; // 演出の持続時間 (3.0秒)
};