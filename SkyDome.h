#pragma once
#include "ModelObject.h"
#include "Transform.h"
#include "Camera.h"
#include "DebugCamera.h"

class SkyDome
{
public:
	void Initialize(ModelObject* model);

	void UpDate();

	void Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	ModelObject* model_;
	Transform transform_;
	Transform uvTransform_;
};

