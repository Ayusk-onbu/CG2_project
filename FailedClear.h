#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class FailedClear
{
public:
	void Initialize(ModelObject* modelClear, ModelObject* modelFailed,CameraBase* camera);
	void Initialize();
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
public:
	void Move(float t);
	void SetIsClear(bool clear) { isClear_ = clear; }
private:
	ModelObject* modelClear_;
	ModelObject* modelFailed_;
	ModelObject* model_;

	bool isClear_ = false;

	CameraBase* camera_;
	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
};

