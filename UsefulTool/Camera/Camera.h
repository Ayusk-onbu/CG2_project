#pragma once
#include "Structures.h"
#include "Quaternion.h"
#include "CameraController.h"

#include <vector>
#include <memory>

struct Projection {
	float fovY;// 画角
	float aspectRatio;// アスペクト比
	float nearClip;// 手前の面
	float farClip;// 奥の面
};

class Camera
{
public:
	Camera();
public:
	void Initialize();
	void Update();
	Matrix4x4 DrawCamera(const Matrix4x4& world);
	Matrix4x4 DrawUI(const Matrix4x4& world);

public:
	Vector3 CalculateRight();
	Vector3 CalculateUp();
	void AddControllers(CameraType cameraTypes);

public:
	float& GetTheta() { return theta_; }
	float& GetPhi() { return phi_; }
	float& GetRadius(){ return radius_; }
	Vector3& GetTranslation() { return translation_; }
	Matrix4x4& GetViewProjectionMatrix(){ return viewProjectionMatrix_; }
	Projection& GetProjection() { return projection_; }

public:
	void SetTheta(const float& theta) { theta_ = theta; }
	void SetPhi(const float& phi) { phi_ = phi; }

private:
	void AddController(CameraType cameraType);
public:

	// 追従対象
	Vector3 targetPos_;
	// 何こいつ
	Matrix4x4 worldMat_;
	// ワールドを軸とした上
	Vector3 up_;
	// カメラの軸たち
	Vector3 xAxis_;
	Vector3 yAxis_;
	Vector3 zAxis_;

protected:
	float theta_;// X軸回転
	float phi_;// Y軸回転
	float radius_;// targetまでの距離

	// Scale
	Vector3 scale_;
	// x,y,z軸周りのローカル回転角
	Vector3 rotation_ = { 0,0,0 };
	// ローカル座標
	Vector3 translation_ = { 0,0,-50 };


	Projection projection_;
	Matrix4x4 viewProjectionMatrix_;

private:
	std::vector<std::unique_ptr<CameraController>>controllers_;
	CameraType useControllerType_;
};

