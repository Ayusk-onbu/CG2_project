#pragma once
#include "Structures.h"
#include "../Base/PrimitiveBaseModel.h"

struct GrassForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
	float windPhase; // 草ごとに揺れるタイミングをズラすためのフェーズ値
	float padding[3];
};

struct GrassObjectData {
	WorldTransform worldTransform;
	Vector4 color;
	float windPhase; // 0.0 ～ 6.28 などのランダム値
};

class Grass :
	public PrimitiveBaseModel<GrassForGPU, GrassObjectData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance) override;

private:
	GrassForGPU ConvertToGPUData(const GrassObjectData& data) override;
};