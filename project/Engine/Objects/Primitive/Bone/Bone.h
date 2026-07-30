#pragma once
#include "Structures.h"
#include "../Base/PrimitiveBaseModel.h"
#include "ModelData.h"

// GPUに送るデータ構造
struct BoneForGPU {
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};

// CPU側のインスタンス用データ構造
struct BoneObjectData {
    Matrix4x4 worldMatrix; // 算出されたボーン単体のワールド行列
    Vector4 color;
};

class BoneDrawer : public PrimitiveBaseModel<BoneForGPU, BoneObjectData> {
public:
    void Initialize(Fngine* engine, uint32_t numInstance) override;

    // Skeleton 全体を一度に描画キューへ追加する便利関数
    void AddSkeleton(const Skeleton& skeleton, const Matrix4x4& modelWorldMatrix, const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
    BoneForGPU ConvertToGPUData(const BoneObjectData& data) override;

    // 親Jointと子Jointの位置からボーン用の変換行列を作成するヘルパー
    Matrix4x4 CalculateBoneMatrix(const Vector3& startPt, const Vector3& endPt);
};