#define NOMINMAX
#include "Player.h"
#include "Matrix4x4.h"
#include <cassert>
#include <numbers>
#include <algorithm>
#include <array>
#include "InputManager.h"
#include "MapChip.h"
#include "Enemy.h"

void Player::Initialize(ModelObject& model,CameraBase& camera, Vector3 translate) {
	model_ = &model;
	camera_ = &camera;
	transform_ = { {1.0f, 1.0f, 1.0f}, 
				   {0.0f, 0.0f, 0.0f}, 
				   {0.0f, 0.0f, 0.0f} };
	transform_.translate = translate;
	uvTransform_ = { {1.0f, 1.0f, 1.0f}, 
				     {0.0f, 0.0f, 0.0f},
					 {0.0f, 0.0f, 0.0f} };
}

void Player::Update() {
	// 着地フラグ
		//bool landing = false;
	////////////////////////////////////////////////////
#pragma region ①移動入力
	Player::Move();
#pragma endregion
	////////////////////////////////////////////////////
#pragma region ②移動量を加味して衝突判定する
		// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.displacement = velocity_;

	IsMapChipHitFunction(collisionMapInfo);
#pragma endregion
	////////////////////////////////////////////////////
#pragma region ③判定結果を反映して移動させる
	Player::MapChipMove(collisionMapInfo);
#pragma endregion
	////////////////////////////////////////////////////
#pragma region ④天井に接触している場合の処理
	Player::DoOnHandingCeiling(collisionMapInfo);
#pragma endregion
	////////////////////////////////////////////////////
#pragma region ⑤壁に接触している場合の処理

#pragma endregion
////////////////////////////////////////////////////
#pragma region ⑥設置状態の切替
	// 着地判定
	ChangeOnGroundFlag(collisionMapInfo);
#pragma endregion
	////////////////////////////////////////////////////
#pragma region ⑦旋回制御
	// 回転
	if (turnTimer_ > 0.0f) {
		// 旋回時間を減算
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> *3.0f / 2.0f };
		// 状況に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		transform_.rotate.y =
			// 旋回開始時の角度と目的地の角度を補間する
			turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * (1.0f - turnTimer_ / kTimeTurn);
	}
#pragma endregion
	////////////////////////////////////////////////////
}

void Player::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	if (model_ && !isDead_) {
		Matrix4x4 world = Matrix4x4::Make::Affine(transform_.scale, transform_.rotate, transform_.translate);

		Matrix4x4 uv = Matrix4x4::Make::Scale(uvTransform_.scale);
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::RotateZ(uvTransform_.rotate.z));
		uv = Matrix4x4::Multiply(uv, Matrix4x4::Make::Translate(uvTransform_.translate));

		model_->SetWVPData(camera_->DrawCamera(world), world, uv);
		model_->Draw(command, pso, light, tex);
	}
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;
}

void Player::Move() {
	// 設置上程なら
	if (onGround_) {
		// 移動入力
		// 左右移動操作
		// 左右加速
		Vector3 acceleration = {};
		if (InputManager::GetKey().PressKey(DIK_RIGHT)) {
			if (velocity_.x < 0.0f) {
				// 速度と逆方向に入力中は減速
				velocity_.x += (1.0f - kAttenuation);
			}
			acceleration.x += kAcceleration;
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				// 旋回開始時の角度を保存
				turnFirstRotationY_ = transform_.rotate.y;
				// 旋回タイマーをリセット
				turnTimer_ = kTimeTurn;
			}
		}
		else if (InputManager::GetKey().PressKey(DIK_LEFT)) {
			if (velocity_.x > 0.0f) {
				// 速度と逆方向に入力中は減速
				velocity_.x -= (1.0f - kAttenuation);
			}
			acceleration.x -= kAcceleration;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				// 旋回開始時の角度を保存
				turnFirstRotationY_ = transform_.rotate.y;
				// 旋回タイマーをリセット
				turnTimer_ = kTimeTurn;
			}
		}
		else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		// 加速/減速
		velocity_.x += acceleration.x;
		velocity_.y += acceleration.y;
		velocity_.z += acceleration.z;

		// 速度制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

#pragma region Jump
		if (InputManager::GetKey().PressKey(DIK_UP)) {
			// ジャンプ
			velocity_.x += 0.0f;
			velocity_.y += kJumpAcceleration;
			velocity_.z += 0.0f;
		}
#pragma endregion
	}
	else {
		// 落下速度
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

}

void Player::IsMapChipHitFunction(CollisionMapInfo& collisionMapInfo)
{
	Player::IsMapChipHitFunctionTop(collisionMapInfo);
	Player::IsMapChipHitFunctionBottom(collisionMapInfo);
	Player::IsMapChipHitFunctionLeft(collisionMapInfo);
	Player::IsMapChipHitFunctionRight(collisionMapInfo);
}


void Player::IsMapChipHitFunctionTop(CollisionMapInfo& collisionMapInfo) {
	// 上昇中か否か
	if (collisionMapInfo.displacement.y <= 0) {
		return;
	}

	// 移動後の４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + collisionMapInfo.displacement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChip::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒットしていたら
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

		// 現在座標が壁の外化判定
		MapChip::IndexSet indexSetNow;
		// 移動前の４つの角の座標
		std::array<Vector3, kNumCorner> positionsNow;
		for (uint32_t i = 0; i < positionsNow.size(); ++i) {
			positionsNow[i] = CornerPosition(transform_.translate, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsNow[kRightTop]);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float displacementY = (rect.bottom - transform_.translate.y) - (0.4f + kBlank);
			collisionMapInfo.displacement.y = std::max(0.0f, (displacementY));
			// 天井に当たったことを記録する
			collisionMapInfo.isCeilingHitFlag = true;
		}
	}
}
void Player::IsMapChipHitFunctionBottom(CollisionMapInfo& collisionMapInfo) {
	// 下方向
	if (collisionMapInfo.displacement.y >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + collisionMapInfo.displacement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChip::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒットしていたら
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

		// 現在座標が壁の外化判定
		MapChip::IndexSet indexSetNow;
		// 移動前の４つの角の座標
		std::array<Vector3, kNumCorner> positionsNow;
		for (uint32_t i = 0; i < positionsNow.size(); ++i) {
			positionsNow[i] = CornerPosition(transform_.translate, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsNow[kRightBottom]);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float displacementY = (rect.top - transform_.translate.y) + (0.4f + kBlank);
			collisionMapInfo.displacement.y = std::min(0.0f, (displacementY));
			// 地面に当たったことを記録する
			collisionMapInfo.isGroundOnFlag = true;
		}
	}
}
void Player::IsMapChipHitFunctionLeft(CollisionMapInfo& collisionMapInfo) {
	// 左移動できるかどうか
	if (collisionMapInfo.displacement.x > 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + collisionMapInfo.displacement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 左の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChip::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒットしていたら
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外化判定
		MapChip::IndexSet indexSetNow;
		// 移動前の４つの角の座標
		std::array<Vector3, kNumCorner> positionsNow;
		for (uint32_t i = 0; i < positionsNow.size(); ++i) {
			positionsNow[i] = CornerPosition(transform_.translate, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsNow[kLeftBottom]);

		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float displacementX = (rect.left - transform_.translate.x) + (0.4f + kBlank);
			collisionMapInfo.displacement.x = std::max(0.0f, (displacementX));
			// 壁に当たったことを記録する
			collisionMapInfo.isWallTouchFlag = true;
		}
	}
}
void Player::IsMapChipHitFunctionRight(CollisionMapInfo& collisionMapInfo) {
	// 右移動できるかどうか
	if (collisionMapInfo.displacement.x <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + collisionMapInfo.displacement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 右の当たり判定を行う
	bool hit = false;
	// 右下点の判定
	MapChip::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒットしていたら
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

		// 現在座標が壁の外化判定
		MapChip::IndexSet indexSetNow;
		// 移動前の４つの角の座標
		std::array<Vector3, kNumCorner> positionsNow;
		for (uint32_t i = 0; i < positionsNow.size(); ++i) {
			positionsNow[i] = CornerPosition(transform_.translate, static_cast<Corner>(i));
		}
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(positionsNow[kRightBottom]);

		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChip::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float displacementX = (rect.right - transform_.translate.x) - (0.4f + kBlank);
			collisionMapInfo.displacement.x = std::min(0.0f, (displacementX));
			// 壁に当たったことを記録する
			collisionMapInfo.isWallTouchFlag = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
		{(kWidth / 2.0f),  -kHeight / 2.0f, 0}, // 右下
		{(-kWidth / 2.0f), -kHeight / 2.0f, 0},	// 左下
		{(kWidth / 2.0f),  kHeight / 2.0f,  0}, // 右上
		{(-kWidth / 2.0f), kHeight / 2.0f, 0}	// 左上
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

// ③判定結果を反映して移動させる
void Player::MapChipMove(const CollisionMapInfo& collisionMapInfo) {
	// 移動
	transform_.translate = transform_.translate + collisionMapInfo.displacement;
}

// ④天井に接触している場合の処理
void Player::DoOnHandingCeiling(const CollisionMapInfo& info) {
	// 天井に当たっていたら
	if (info.isCeilingHitFlag) {
		velocity_.y = 0;
	}
}

// ⑤壁に接触している場合の処理
void Player::DoTouchWall(const CollisionMapInfo& info) {
	// 壁に当たっていたら
	if (info.isWallTouchFlag) {
		// 横方向の速度をゼロにする
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

// ⑥設置状態の切替
void Player::ChangeOnGroundFlag(const CollisionMapInfo& info) {
	// 地面に接触している場合
	if (onGround_) {
		// 設置状態の処理
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
		else {
			// 落下判定
			// 移動後の４つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(transform_.translate + info.displacement, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下の判定
			MapChip::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3{ 0,-kBlankMapChip,0 });
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3{ 0, -kBlankMapChip, 0 });
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 落下中なら空中状態に切替
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	}
	else {
		// 空中状態の処理
		// 着地
		if (info.isGroundOnFlag) {
			onGround_ = true;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}