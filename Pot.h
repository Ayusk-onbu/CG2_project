#pragma once
#include "Fngine.h"
#include "ModelObject.h"
#include "Collider.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class Pot
	: public Collider
{
public:
	void Initialize(ModelObject* model, CameraBase* camera,Fngine&fngine);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
public:
	void OnCollision()override;
	const Vector3 GetWorldPosition()override;
	void SetPosition(const Vector3& pos) { worldTransform_.set_.Translation(pos); }
	void SetColor(const Vector4& color) { model_->SetColor(color); }
private:
	std::unique_ptr<ModelObject>model_;
	CameraBase* camera_;

	WorldTransform worldTransform_;
	WorldTransform uvTransform_;
};

