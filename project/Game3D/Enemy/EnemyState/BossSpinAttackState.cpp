#include "EnemyState.h"
#include "Enemy.h"

void BossSpinAttackState::Initialize() {
	if (!boss_)return;
	timer_ = 0.0f;
	// 攻撃範囲を可視化
	boss_->SetIsViewAttack(true);
}

void BossSpinAttackState::Update() {
    if (!boss_) return;

    const float deltaTime = 1.0f / 60.0f;
    timer_ += deltaTime;

    // 攻撃判定の制御
    if (timer_ >= activeStart_ && timer_ <= activeEnd_) {
        // ボスの現在位置を中心に、全方位(360度)に届く大きな判定を出す
        boss_->EnableAttackHitBox(true, boss_->GetPosition(), spinRadius_);

        // 演出用：ボス自身を高速回転させる（見た目の説得力）
        currentAngle_ += 40.0f; // 毎フレーム回転
        // ※本来はQuaternionで設定するが、ここでは概念のみ

    }
    else {
        boss_->EnableAttackHitBox(false, { 0,0,0 }, 0.0f);
    }

    // 2. 終了判定
    if (timer_ >= maxDuration_) {
        boss_->ChangeState(new BossDecisionState());
        return;
    }
}

void BossSpinAttackState::Exit() {
    if (!boss_) return;
    boss_->EnableAttackHitBox(false, { 0,0,0 }, 0.0f);
    boss_->SetIsViewAttack(false);
}