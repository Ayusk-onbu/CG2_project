#pragma once
#include "SpriteObject.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class Sprite
{
public:
	void Initialize(SpriteObject& obj,CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	SpriteObject* obj_ = nullptr;
	CameraBase* camera_ = nullptr;
	WorldTransform worldTransform_;
	WorldTransform uvWorldTransform_;
};

