#pragma once
#include "Structures.h"

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

class Camera
{
private:
	struct CameraInfo {
		// Scale
		Vector3 scale_;
		// x,y,z軸周りのローカル回転角
		Vector3 rotation_ = { 0,0,0 };

		Matrix4x4 matRot_;
		// ローカル座標
		Vector3 translation_ = { 0,0,-50 };
	};
	struct Projection {
		float fovY;// 画角
		float aspectRatio;// アスペクト比
		float nearClip;// 手前の面
		float farClip;// 奥の面
	};
public:
#pragma region 初期化(Initialize)
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
#pragma endregion
#pragma region 更新(UpData)
	/// <summary>
	/// 更新
	/// </summary>
	void UpData();
#pragma endregion

	Matrix4x4 DrawCamera(const Matrix4x4& world);

	void GetRotateDelta(float speedX, float speedY) {
		Matrix4x4 matRotDelta = Matrix4x4::Make::Identity();
		matRotDelta = Matrix4x4::Multiply(matRotDelta, Matrix4x4::Make::RotateX(speedX));
		matRotDelta = Matrix4x4::Multiply(matRotDelta, Matrix4x4::Make::RotateY(speedY));
		camera_.matRot_ = Matrix4x4::Multiply(camera_.matRot_, matRotDelta);
	}

public:
	bool IsPivot_;
	float speed_;
	float sensitivity_;
	Vector2 rotateSpeed_;
public:
	CameraInfo camera_;
	Projection projection_;
	Matrix4x4 viewProjectionMatrix_;
};

class CameraController {

	// 座標補間割合
	static inline const float kInterpolationRate = 0.5f;// 1フレームで目標に近づく割合
	//速度掛け率
	static inline const float kVelocityBias = 1.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect movementMargin = { -50.0f, 50.0f, -50.0f, 50.0f };

public:

	void Initialize(Camera* camera);
	void Update();
	void SetTarget(Player* target) { target_ = target; }
	void Reset();
	void SetMovableArea(Rect area) { movableArea_ = area; }

private:
	//ビュープロジェクション
	Matrix4x4 viewProjection_;
	// カメラ
	Camera* camera_ = nullptr;
	// プレイヤー
	Player* target_ = nullptr;
	// 追従対象とカメラの座標の差
	Vector3 targetOffset_ = { 0.0f, 10.0f, -30.0f };
	//カメラ移動範囲
	Rect movableArea_ = { 0, 100, 0, 100 };
	// カメラの目標座標
	Vector3 targetPos_;
};



