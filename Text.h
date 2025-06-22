#pragma once
#include "ModelObject.h"
#include "Transform.h"
#include "Camera.h"
#include "DebugCamera.h"

class Text
{
public:
	void Initialize(ModelObject* model);

	void UpDate();

	void Draw(Camera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(DebugCamera& camera, TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	Transform& GetTranslate() { return transform_; }
private:
	ModelObject* model_;
	Transform transform_;
	Transform uvTransform_;
	float rad_;
};

