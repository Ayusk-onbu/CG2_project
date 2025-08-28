#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"
class Ground
{
public:
	void Initialize(ModelObject* model, CameraBase* camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	ModelObject* model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
};

