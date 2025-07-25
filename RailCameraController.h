#pragma once
#include "WorldTransform.h"
#include "CameraBase.h"

class RailCameraController
{
public:
	void Initialize(CameraBase* camera);
	void Update();
private:
	void Move();
private:
	CameraBase* camera_;
	Vector3 moveSpeed_;
};

