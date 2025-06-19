#pragma once
#include "ObjectBase.h"

class SphereObject
{
public:
	void Initialize(D3D12System& d3d12);

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	void SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv);
private:
	void InitializeResource(D3D12System& d3d12);

	void InitializeData();

	void InitializeVertex();
private:
	ObjectBase object_;
	//分割数
	float kSubdivision_;
	//頂点数 (分割数(縦÷緯度)x分割数(横÷経度)x６)
	uint32_t vertexCount;
};

