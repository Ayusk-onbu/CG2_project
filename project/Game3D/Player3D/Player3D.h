#pragma once
#include "ModelObject.h"

class Player3D
{
public:
	void Initialize(Fngine* fngine);
	void Update();
	void Draw();
private:
	void Move(Vector3& pos);
private:
	std::unique_ptr<ModelObject>obj_;
	Vector3 move_ = { 0,0,0 };
	float speed_ = 1.0f;
	float easeFrame_ = 0.0f;


};

