#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera
{
private:
	struct Camera {
		// Scale
		Vector3 scale_;
		// x,y,z軸周りのローカル回転角
		Vector3 rotation_ = { 0,0,0 };
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

	void MoveForward();

	void MoveBack();

	void MoveRight();

	void MoveLeft();

	void MoveUp();

	void MoveDown();

	void RotatePitch();

	void RotateYaw();

	void RotateRoll();

public:
	float speed_;
	float rotateSpeed_;
private:
	Camera camera_;
	Projection projection_;
	Matrix4x4 viewProjectionMatrix_;
};

