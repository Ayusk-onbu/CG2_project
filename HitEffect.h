#pragma once
#include "Transform.h"
#include "ModelObject.h"
#include "CameraBase.h"

class HitEffect
{
public:

	enum class BEHAVIOR {
		Expand,
		FadeOut,
		Destroy,

		UnKnown,
	};

	void Initialize(Vector3& pos);

	void Update();
	void BehaviorUpdate();
	void BehaviorExpandUpdate();
	void BehaviorFadeOutUpDate();
	void BehaviorDestroyUpdate();

	void BehaviorTransition();

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	static void SetModel(ModelObject* model) { model_ = model;}
	static void SetModel2(ModelObject* model) { model2_ = model; }
	static void SetModel3(ModelObject* model) { model3_ = model; }
	static void SetCamera(CameraBase* camera) { camera_ = camera; }
	void SetPos(Vector3& pos) {
		transform_.translate = pos;
	}

private:

	BEHAVIOR behavior_ = BEHAVIOR::Expand;
	BEHAVIOR behaviorRequest_ = BEHAVIOR::UnKnown;
	float counter_ = 0.0f;

	static ModelObject* model_;
	static ModelObject* model2_;
	float rotate2_;
	static ModelObject* model3_;
	float rotate3_;
	static CameraBase* camera_;
	Transform transform_;
	bool isDraw_;
	Vector4 color_;
};

