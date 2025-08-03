#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class hallway
	:public Collider
{
public:
	void Initialize();
	void Update();
	void Draw();
};

