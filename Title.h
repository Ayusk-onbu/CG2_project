#pragma once
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class Title
	:public Collider
{
public:
	void Initialize(ModelObject* model, CameraBase* camera);
	void Initialize();
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
public:
	void Move(float t);
	void OnCollision()override;
	const Vector3 GetWorldPosition()override;
private:
	ModelObject* model_;
	CameraBase* camera_;

	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
};
