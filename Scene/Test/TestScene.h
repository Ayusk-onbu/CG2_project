#pragma once
#include "Scene.h"

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
};

