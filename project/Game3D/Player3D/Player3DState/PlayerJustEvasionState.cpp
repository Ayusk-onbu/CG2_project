#include "PlayerState.h"
#include "Player3D.h"

void PlayerJustEvasionState::Initialize() {
	if (!player_)return;
	// 初期化処理
	// [ タイマーの初期化 ]
	evasionTimer_ = 0.0f;

	// 回避方向の設定
	Vector3 inputVector = player_->Move();

	evasionDirection_ = Normalize(inputVector);

	// 無敵状態の設定
	player_->SetInvulnerable(true);
}

void PlayerJustEvasionState::Update() {
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

	// Just回避時間終了
	// 何もしていなかったらそのまま終わる
	if (evasionTimer_ >= evasionDuration_) {
		// 待機 State に戻る
		player_->ChangeState(new PlayerStopState());
		return;
	}
}

void PlayerJustEvasionState::Exit() {

}
