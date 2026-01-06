#include "PlayerState.h"
#include "Player3D.h"

void PlayerDashState::Initialize() {
	
	// もしダッシュできない状態ならどうする？
	if (player_->GetStamina() < 0.0f) {
		// 疲れた状態への遷移？ or 歩く？

	}
	// ダッシュの先行入力をクリア
	player_->ClearDashBuffer();

	// 開始時のスタミナの消費
	player_->DrainStamina(initial_staminaCost_);

	// 回復をブロック
	player_->BlockStaminaRecovery();

	// 加速の起点
	currentSpeedMultiplier_ = initial_speedMultiplier_;
	player_->SetSpeedMultiplier(currentSpeedMultiplier_);
}

void PlayerDashState::Update() {

	bool isMove = player_->IsMoving();
	const float deltaTime = 1.0f / 60.0f;

	// 加速処理
	if (currentSpeedMultiplier_ < maxSpeedMultiplier_) {
		currentSpeedMultiplier_ += accelerationRate_ * deltaTime * 2.0f;
		if (currentSpeedMultiplier_ > maxSpeedMultiplier_) {
			// 最大を超えたときの処理
			currentSpeedMultiplier_ = maxSpeedMultiplier_;
		}
	}
	// 適用
	player_->SetSpeedMultiplier(currentSpeedMultiplier_);

	// スタミナを減少
	player_->DrainStamina(staminaDrainRate_ * deltaTime);

	//	ここから状態遷移チェック
	bool has_stamina = player_->GetStamina() > 0.0f;

	if (has_stamina == false) {
		// スタミナ切れ
		player_->ChangeState(new PlayerExhaustedState());
		return;
	}

	if (player_->CanDash() == false) {
		player_->ChangeState(new PlayerStopState());
		return;
	}

	if (player_->IsOnGround() == false) {
		// 空中にいる場合の処理
		player_->ChangeState(new PlayerFallState());
		return;
	}

	if (player_->HasAttackBuffer()) {
		// 攻撃
		player_->ChangeState(new PlayerAttackState(0));
		return;
	}
	
	ImGuiManager::GetInstance()->Text("DashState");
}

void PlayerDashState::Exit() {
	// スタミナの回復ブロックを解除
	player_->UnblockStaminaRecovery();

	// 速度を戻す
	player_->SetSpeedMultiplier(1.0f);

	// なにかあればここに書く

}