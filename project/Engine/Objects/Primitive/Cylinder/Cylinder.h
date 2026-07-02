#pragma once
#include "Structures.h"
#include "VertexData.h"
#include "../Base/PrimitiveBaseModel.h"

#include <vector>
/// <summary>
/// Cylinderのデータ構造：分割数、上半径、下半径、高さ
/// </summary>
struct CylinderData {
	uint32_t divide = 32;// 分割数
	float topRadius = 1.0f;// 上半径
	float bottomRadius = 0.2f;// 下半径
	float height = 1.0f;// 高さ
};
// GPU用
struct PrimitiveCylinderForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 uvTransform;
	Vector4 color;
};
// CPU用
struct PrimitiveCylinderData {
	WorldTransform worldTransform;
	WorldTransform uvTransform;
	Vector4 color;
};

std::vector<VertexData> MakeObjectVertices(const CylinderData& data);

std::vector<uint32_t> MakeObjectIndices(const CylinderData& data);

class PrimitiveCylinder :
	public PrimitiveBaseModel<PrimitiveCylinderForGPU, PrimitiveCylinderData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance)override;
private:
	PrimitiveCylinderForGPU ConvertToGPUData(const PrimitiveCylinderData& data)override;

};