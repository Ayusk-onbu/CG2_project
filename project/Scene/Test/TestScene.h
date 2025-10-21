#pragma once
#include "Scene.h"
#include "Collider.h"
#include "Quaternion.h"
#include "Player3D.h"

class TestScene
	: public Scene
{
public:
	TestScene() = default;
	~TestScene() = default;
public:

	// =================================
	// Override Functions
	// =================================

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Player3D player_;
};

