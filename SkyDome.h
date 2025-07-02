#pragma once
#include "Transform.h"
#include "ModelObject.h"
#include "CameraBase.h"

class SkyDome
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	Transform transform_;
	Transform uvTransform_;
	ModelObject* model_;
	CameraBase* camera_;
};

