#pragma once
#include "CameraBase.h"

// 前方宣言
class Player;

/// <summary>
/// left right bottom top
/// </summary>
struct Rect {
	float left = 0.0f;//  左端
	float right = 1.0f;//  右端
	float bottom = 0.0f;//  下端
	float top = 1.0f;//  上端
};

class CameraController {

	// 座標補間割合
	static inline const float kInterpolationRate = 0.5f;// 1フレームで目標に近づく割合
	//速度掛け率
	static inline const float kVelocityBias = 1.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect movementMargin = { -50, 50, -50, 50 };

public:

	void Initialize(CameraBase* camera);
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	//ビュープロジェクション
	Matrix4x4 viewProjection_;
	// カメラ
	CameraBase* camera_ = nullptr;
	// プレイヤー
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差
	Vector3 targetOffset_ = { 0.0f, 0.0f, -15.0f };
	//カメラ移動範囲
	Rect movableArea_ = { 0, 100, 0, 100 };
	// カメラの目標座標
	Vector3 targetPos_;
};


