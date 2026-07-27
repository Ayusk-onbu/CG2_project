#pragma once
#include "Structures.h"
#include "../Base/PrimitiveBaseModel.h"

struct ColumnForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 uvTransform;
	Vector4 color;
	uint32_t colorTextureIndex;  // カラーテクスチャのインデックス
	uint32_t normalTextureIndex; // 法線テクスチャのインデックス
	float padding[2];            // 16バイトアライメント調整
};

struct ColumnObjectData {
	WorldTransform worldTransform;
	WorldTransform uvTransform;
	Vector4 color;
	std::string colorTextureName;  // 例: "Stone_Wall"
	std::string normalTextureName; // 例: "Stone_Normal"
};

class Column :
	public PrimitiveBaseModel<ColumnForGPU, ColumnObjectData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance) override;

private:
	ColumnForGPU ConvertToGPUData(const ColumnObjectData& data) override;
};