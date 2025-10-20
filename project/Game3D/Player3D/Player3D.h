#pragma once
#include "ModelObject.h"

class Player3D
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	void Draw();
private:
	std::unique_ptr<ModelObject>obj_;
	Vector3 move_ = { 0,0,0 };
	int han;
};

