#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"

struct Camera {
	// Scale
	Vector3 scale_;
	// x,y,z軸周りのローカル回転角
	Vector3 rotation_ = { 0,0,0 };
	// ピボット回転用
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

class CameraBase
{

public:
	void Initialize();
	void UpDate();

	Matrix4x4 DrawCamera(const Matrix4x4& world);
	Matrix4x4 DrawUI(const Matrix4x4& world);

	const float& GetRadius() const { return radius_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }
	void SetTargetPos(const Vector3& target);
	void SetTheta(const float& theta) { theta_ = theta; }
	void SetPhi(const float& phi) { phi_ = phi; }
private:
	Vector3 CalculateRight();
	Vector3 CalculateUp();

public:

	Vector3 targetPos_;
	Matrix4x4 worldMat_;
	Vector3 up_;

private:
	float theta_;
	float phi_;
	float radius_;

	Vector3 xAxis_;
	Vector3 yAxis_;
	Vector3 zAxis_;

	Camera camera_;
	Projection projection_;
	Matrix4x4 viewProjectionMatrix_;
};

