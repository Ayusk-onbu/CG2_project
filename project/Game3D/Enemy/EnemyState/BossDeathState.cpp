#include "EnemyState.h"
#include "Enemy.h"

void BossDeathState::Initialize()
{
	if (!boss_) return;

	deathTimer_ = 0.0f;

	// 1. 死亡時の処理
	// ボス本体の当たり判定を完全に無効化し、それ以上のダメージを防ぐ
	// boss_->bodyCollider_->SetMyType(COL_NONE); 

	// 2. 死亡演出を開始
	// boss_->PlayAnimation("Death");
	// boss_->SpawnParticleEffect("Explosion"); 

	// 3. アイテムドロップ（後で実装）
	// boss_->DropLoot(); 

	// 4. 移動・攻撃処理を停止
	boss_->SetMovement({ 0.0f, 0.0f, 0.0f });
}

void BossDeathState::Update()
{
	if (!boss_) return;

	const float deltaTime = 1.0f / 60.0f;
	deathTimer_ += deltaTime;

	// 1. 移動は完全に停止
	boss_->SetMovement({ 0.0f, 0.0f, 0.0f });

	// 2. 演出タイマーのチェック
	if (deathTimer_ >= maxDeathDuration_)
	{
		// 演出終了後、シーン管理クラスなどに通知してリザルト画面に遷移
		// SceneManager::ChangeScene(SceneType::Result); 

		// 倒されたボス自身はこれ以上 Update する必要がないため、
		// 親のEnemyクラス内でこのオブジェクトを破棄するフラグを立てても良い
		// boss_->SetIsFinished(true); 
	}

	ImGuiManager::GetInstance()->Text("BossState : Death");
}