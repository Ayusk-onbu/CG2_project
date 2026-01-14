#include "PlayerState.h"
#include "Player3D.h"

void PlayerStepEvasionState::Initialize() {
	// Player のチェック
	if(!player_)return;

	// 初期化処理
	evasionTimer_ = 0.0f;

	// 回避方向の設定
	Vector3 inputVector = player_->Move();

	if (player_->IsMoving()) {
		// 入力がある場合、入力方向に回避
		evasionDirection_ = Normalize(inputVector);
	}
	else {
		// 入力がない場合、プレイヤーの後方向に回避
		evasionDirection_ = player_->GetForwardVector();
		evasionDirection_ = -evasionDirection_;
	}

	// 無敵状態の設定
	player_->SetInvulnerable(true);

	// 回避アニメーションを設定

	// スタミナを減らす
	player_->BlockStaminaRecovery();
	player_->DrainStamina(initial_staminaCost_);
}

void PlayerStepEvasionState::Update() {
	// プレイヤーチェック
	if (!player_) return;
	// Δタイム
	const float deltaTime = 1.0f / 60.0f;
	evasionTimer_ += deltaTime;

	// 移動処理
	// [ 回避を適用 ]
	player_->Move() = evasionDirection_;
	// [ スピードを設定 ]
	player_->SetSpeedMultiplier((evasionDistance_ / evasionDuration_));

	// 回避時間終了
	if (evasionTimer_ >= evasionDuration_) {
		// 待機 State に戻る
		player_->ChangeState(new PlayerStopState());
		return;
	}

	// ジャスト回避の処理
	if (evasionTimer_ < evasionDuration_) {
		player_->SetJustEvasion(true);
	}
	else {
		player_->SetJustEvasion(false);
	}

	ImGuiManager::GetInstance()->Text("StepEvasionState");
}

void PlayerStepEvasionState::Exit() {
	// 無敵状態を解除
	player_->SetInvulnerable(false);

	player_->SetJustEvasion(false);

	// 速度を元に戻す
	player_->SetSpeedMultiplier(1.0f);

	// スタミナ回復を解除
	player_->UnblockStaminaRecovery();
}