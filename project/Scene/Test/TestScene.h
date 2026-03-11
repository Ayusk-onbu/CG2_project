#pragma once
#include "Scene.h"
#include "Collider.h"
#include "Quaternion.h"
#include "Player3D.h"
#include "Particle.h"
#include "UIContainer.h"
#include "Ground.h"

import Hermite;
using namespace MathUtils;

class TestScene
	: public Scene
{
public:
	TestScene() = default;
	~TestScene()override;
public:

	// =================================
	// Override Functions
	// =================================

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PauseUpdate()override;
	void PauseDraw()override;
	bool CanPause()const override { return true; }

private:
	void NodeImGui();
	int draggedNodeIndex = -1;
	int draggedHandleType = 0; // 0:Position, 1:TangentIn, 2:TangentOut
	std::vector<Spline::Node<Vector3>>nodes_;
	
	UIContainer container_;
	UIHAnimation animation_;

	std::unique_ptr<Player3D>player_;
	std::unique_ptr<Particle>particle_;

	std::unique_ptr<ConvenienceModel>ground_;
};

