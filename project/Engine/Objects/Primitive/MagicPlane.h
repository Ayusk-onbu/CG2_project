#pragma once
#include "Base/PrimitiveBaseModel.h"
#include "VertexData.h"
#include "Fngine.h"

struct MagicCircleConfig
{
    float DrawThreshold = 0.0f; // 描画具合 (0.0 ～ 1.0)
    float DissolveThreshold = 1.0f; // 消失具合 (0.0 ～ 1.0)
    float RotationAngle = 0.0f; // 回転角度 (ラジアン)
    float Padding = 0.0f; // 16バイトアライメント用
};

enum class MagicState {
	Drawing,  // 筆を追うように描かれている状態
	Active,   // 全開で光って待機している状態
	Dissolve  // 溶けて消える状態
};

// GPU用
struct MagicCircleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 uvTransform;
	Vector4 color;
};
// CPU用
struct MagicCircleData {
	WorldTransform worldTransform;
	WorldTransform uvTransform;
	Vector4 color;
};

class MagicCircle :
	public PrimitiveBaseModel<MagicCircleForGPU, MagicCircleData>
{
public:
	void Initialize(Fngine* engine, uint32_t numInstance)override;
	void Update();
private:
	MagicCircleForGPU ConvertToGPUData(const MagicCircleData& data)override;
	std::unique_ptr<ConstantBuffer<MagicCircleConfig>>gpuConfig_;
	MagicState currentState = MagicState::Drawing;
	float activeTimer = 0.0f;    // 待機時間の計測用
};