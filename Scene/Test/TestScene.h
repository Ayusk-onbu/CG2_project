#pragma once
#include "Scene.h"
#include "Collider.h"
#include "Quaternion.h"
#include "Player3D.h"
#include "Cameras.h"

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
	// カメラ
	Camera* useCamera_;
	// 第三者視点
	std::unique_ptr<Camera>mainCamera_;

	
};

