#pragma once
#include "ObjectBase.h"

class SpriteObject
{
public:
	void Initialize(D3D12System& d3d12, float width, float height);

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw2(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);

	void SetWVPData(Matrix4x4 WVP,Matrix4x4 world,Matrix4x4 uv);
	void SetColor(Vector4 color) { object_.materialData_->color = color; }
private:
	void InitializeResource(D3D12System& d3d12);

	void InitializeData();

	void InitializeVertex(float width,float height);
private:
	ObjectBase object_;
};

