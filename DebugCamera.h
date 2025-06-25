#pragma once
#include "Vector2.h"
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

	Matrix4x4 DrawMirrorCamera(const Matrix4x4& world, Vector3 mirrorPos, Vector3 MirrorNormal);

	void Zoom();





	void MoveRight();

	void MoveLeft();

	void MoveUp();

	void MoveDown();

	void RotatePitch();

	void RotateYaw();

	void RotateRoll();

	void GetRotateDelta(float speedX,float speedY) {
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
private:
	Camera camera_;
	Projection projection_;
	Matrix4x4 viewProjectionMatrix_;
};

Vector3 ChangeMirror(Vector3 pos,Vector3 mirrorPos,Vector3 MirrorNormal);

