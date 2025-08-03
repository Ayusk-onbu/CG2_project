#pragma once
#include "SphereObject.h"
#include "WorldTransform.h"
#include "CameraBase.h"
class SphereObj
{
public:
	void Initialize(SphereObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
protected:
	SphereObject* model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;
	WorldTransform uvWorldTransform_;
};

