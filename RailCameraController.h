#pragma once
#include "WorldTransform.h"
#include "CameraBase.h"
#include <vector>

class RailCameraController
{
public:
	void Initialize(CameraBase* camera);
	void Update();

	void SetRailPoints(const std::vector<Vector3>& points) {
		railPoints_ = &points;
	}
private:
	void Move();
private:
	CameraBase* camera_;
	Vector3 preTarget_;
	Vector3 moveSpeed_;
	const std::vector<Vector3>* railPoints_; // レール上のポイント
	uint32_t time_;
	uint32_t timer_;
};

