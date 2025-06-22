#pragma once
#include "ModelObject.h"
#include "Transform.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "AABB.h"

class MapChipField;
class Enemy;

enum class LRDirection {
	kRight,
	kLeft,
};
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上

	kNumCorner    // 要素数
};
struct CollisionMapInfo {
	bool isCeilingHitFlag = false;// 天井
	bool isGroundOnFlag = false;// 地面
	bool isWallTouchFlag = false;// 壁
	Vector3 displacement;// 移動量
};

class Player
{
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.0001f;// 微小な余白
	static inline const float kBlankMapChip = 0.0001f; // 微小な余白

	static inline const float kAcceleration = 0.025f;
	static inline const float kAttenuation = 0.9f;
	static inline const float kAttenuationLanding = 0.95f;
	static inline const float kAttenuationWall = 0.8f;
	static inline const float kLimitRunSpeed = 0.5f;
	//旋回時間
	static inline const float kTimeTurn = 1.0f;

	//重力加速度
	static inline const float kGravityAcceleration = 0.01f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ加速度
	static inline const float kJumpAcceleration = 0.2f;
public:
	void Initialize(ModelObject* model,Vector3& pos);

	void UpDate();

	void Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	Transform& GetWorldTransform() { return transform_; }
	const Vector3& GetVelocity() const { return velocity_; }
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	void OnCollision(const Enemy* enemy);
	bool GetIsDeathFlag() { return isDeath_; }
private:

	Vector3 velocity_ = {};
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//設置上程フラグ
	bool onGround_ = true;
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	ModelObject* model_;
	Transform transform_;
	Transform uvTransform_;

	bool isDeath_;

private:

	void Move();
	void MapChipMove(const CollisionMapInfo& collisionMapInfo);
	void DoOnHandingCeiling(const CollisionMapInfo& info);
	void ChangeOnGroundFlag(const CollisionMapInfo& info);
	void DoTouchWall(const CollisionMapInfo& info);
	void IsMapChipHitFunction(CollisionMapInfo& info);
	void IsMapChipHitFunctionTop(CollisionMapInfo& info);
	void IsMapChipHitFunctionBottom(CollisionMapInfo& info);
	void IsMapChipHitFunctionLeft(CollisionMapInfo& info);
	void IsMapChipHitFunctionRight(CollisionMapInfo& info);

};

