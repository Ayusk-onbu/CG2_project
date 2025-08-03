#pragma once
#include "ModelObject.h"
#include "WorldTransform.h"
#include "CameraBase.h"

class OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
protected:
	ModelObject* model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;
	WorldTransform uvWorldTransform_;
};

class Teapot : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
};

class Bunny : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
};

class MultiMesh : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
};

class MultiMaterial : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
};
class Suzanne : public OBJ
{
public:
	void Initialize(ModelObject& model, CameraBase& camera);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	Vector4 color_ = { 1.0f, 0.0f, 0.0f, 1.0f }; // Default color for Suzanne
};

