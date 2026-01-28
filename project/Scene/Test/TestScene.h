#pragma once
#include "Scene.h"
#include "Collider.h"
#include "Quaternion.h"
#include "Player3D.h"
#include "Particle.h"
#include "UIContainer.h"
#include "Ground.h"

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
	UIContainer container_;
	UIHAnimation animation_;

	float toGameTimer_ = 0.0f;
	std::unique_ptr<Player3D>player_;
	std::unique_ptr<SpriteObject>title_;
	std::unique_ptr<SpriteObject>fade_;
	std::unique_ptr<Particle>particle_;

	std::unique_ptr<ConvenienceModel>ground_;
};

