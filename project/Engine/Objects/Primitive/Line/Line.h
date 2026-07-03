#pragma once
#include "Structures.h"
#include "VertexData.h"
#include "../Base/PrimitiveBaseModel.h"

#include <vector>
// CPU側：ゲームロジックから指定するデータ
struct PrimitiveLineData {
	Vector3 startPoint; // 始点座標
	Vector3 endPoint;   // 終点座標
	Vector4 color;      // 線の色
};

// GPU側：StructuredBufferに流し込むデータ
struct PrimitiveLineForGPU {
	Vector4 startPoint; // w = 1.0f
	Vector4 endPoint;   // w = 1.0f
	Vector4 color;
};

std::vector<VertexData> MakeLineVertices();

std::vector<uint32_t> MakeLineIndices();

class PrimitiveLine :
	public PrimitiveBaseModel<PrimitiveLineForGPU, PrimitiveLineData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance)override;
private:
	PrimitiveLineForGPU ConvertToGPUData(const PrimitiveLineData& data)override;

};