#pragma once
#include "ObjectBase.h"

class TriangleObject
{
public:
	void Initialize(D3D12System& d3d12, float width, float height);

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	void SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv);
private:
	void InitializeResource(D3D12System& d3d12);

	void InitializeData();

	void InitializeVertex(float width, float height);
private:
	ObjectBase object_;
};

