#pragma once
#include "../Base/PrimitiveBaseModel.h"

struct PrimitiveSphereForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct PrimitiveSphereData {
	WorldTransform worldTransform;
	Vector4 color;
};

class PrimitiveSphere :
	public PrimitiveBaseModel<PrimitiveSphereForGPU, PrimitiveSphereData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance)override;
private:
	PrimitiveSphereForGPU ConvertToGPUData(const PrimitiveSphereData& data)override;
};