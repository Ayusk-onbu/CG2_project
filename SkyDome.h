#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"

class SkyDome
{
public:
	void Initialize(ModelObject* model,CameraBase* camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

private:
	void UvUpdate();
private:
	ModelObject* model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
};

