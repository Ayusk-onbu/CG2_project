#pragma once
#include "Scene.h"
#include "Collider.h"
#include "Quaternion.h"

class TestScene
	: public Scene
{
public:

	// =================================
	// Override Functions
	// =================================

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Vector4 test_;
	Vector4 test2_;
	Matrix4x4 testM_;

	
	Quaternion rotation_;
	Quaternion rotation2_;
	Vector4 re1_;
	Vector4 re2_;
	Vector4 re3_;
	Vector4 re4_;
	Vector4 re5_;
};

