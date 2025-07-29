#include "LockOn.h"
#include "MathUtils.h"
#include "ImGuiManager.h"

void LockOn::Initialize(SpriteObject& sprite, Texture& tex) {
	sprite_ = sprite;
	texture_ = &tex;
}

void LockOn::Update(Player* player,std::list<Enemy*>& enemies, CameraBase& camera) {
	// ロックオン対象リスト
	std::list<std::pair<float, Enemy*>> targets; // <距離,敵ポインタ>
	// ロックオン判定処理
	for (Enemy* enemy : enemies) {
		//ロックオン判定の実装

		// 敵のワールド座標取得
		//Vector3 enemyPos = enemy->GetWorldPosition();
		// ワールド座標からスクリーン座標に変換
		Matrix4x4 drawSpriteMat = camera.DrawCamera(enemy->GetWorldTransform().mat_);
		Vector4 worldPos = { drawSpriteMat.m[3][0],drawSpriteMat.m[3][1] ,drawSpriteMat.m[3][2],1.0f };
		Vector4 clip = Matrix4x4::Transform(camera.GetViewProjectionMatrix(),worldPos);
		clip.x /= clip.w;
		clip.y /= clip.w;
		clip.z /= clip.w;
		float screenX = (clip.x * 0.5f + 0.0f) * 128.0f;
		float screenY = (clip.y * 0.5f + 0.0f) * 72.0f;
		drawSpriteMat.m[3][0] = screenX;
		drawSpriteMat.m[3][1] = screenY;
		drawSpriteMat.m[3][2] = clip.z;
		// V２に格納
		Vector2 positionScreenV2(drawSpriteMat.m[3][0], drawSpriteMat.m[3][1]);
		// スプライトの中心からの距離
		float distance = Distance(player->Get2DReticlePos(),positionScreenV2);
		// ロックオン距離の限界地
		const float kDistanceLockOn = 50.0f;
		// 2Dレティクルからのスクリーン距離が規定範囲内なら
		if (distance <= kDistanceLockOn) {
			// ロックオンリストに追加
			targets.emplace_back(std::make_pair(distance,enemy));
		}
	}

	targetEnemy_ = nullptr;
	player->targetPos_ = player->GetWorldPosition3DReticle();
	player->isLockOn_ = false; // ロックオン中ではない
	if (!targets.empty()) {
		// 距離で昇順にソート
		targets.sort();
		// 最も近い敵をロックオン
		targetEnemy_ = targets.front().second;
		player->SetTarget(*targetEnemy_);
		player->isLockOn_ = true; // ロックオン中にする
		// ロックオン中の敵のワールド座標を取得
		Vector3 targetPos = targetEnemy_->GetWorldPosition();
		player->targetPos_ = targetPos;
		// スプライトのワールド座標を設定
		Matrix4x4 drawSpriteMat = camera.DrawCamera(targetEnemy_->GetWorldTransform().mat_);
		Vector4 worldPos = { drawSpriteMat.m[3][0],drawSpriteMat.m[3][1] ,drawSpriteMat.m[3][2],1.0f };
		Vector4 clip = Matrix4x4::Transform(camera.GetViewProjectionMatrix(), worldPos);
		clip.x /= clip.w;
		clip.y /= clip.w;
		clip.z /= clip.w;
		float screenX = (clip.x * 0.5f + 0.0f) * 128.0f;
		float screenY = (clip.y * 0.5f + 0.0f) * 72.0f;
		drawSpriteMat.m[3][0] = screenX;
		drawSpriteMat.m[3][1] = screenY;
		drawSpriteMat.m[3][2] = clip.z;
		// スプライトのワールド座標を設定
		sprite_.SetWVPData(drawSpriteMat, targetEnemy_->GetWorldTransform().mat_, Matrix4x4::Make::Identity());
	}
}

void LockOn::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light) {
	sprite_.Draw(command,pso,light,*texture_);
}