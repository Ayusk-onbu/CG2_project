#pragma once
#include "../Base/PrimitiveBaseModel.h"
#include "Material.h"

struct PrimitiveBoxForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct PrimitiveBoxData {
	WorldTransform worldTransform;
	Vector4 color;
};

class PrimitiveBox :
	public PrimitiveBaseModel<PrimitiveBoxForGPU, PrimitiveBoxData>, 
	public ISingleton<PrimitiveBox>
{
	// シングルトン
private:
	PrimitiveBox() = default;// 勝手にNewをされないように
	// 親クラスにPriveteを覗かれてもいいようにカギを渡す役割(上記のようにコンストラクタを触れないのを防ぐため)
	friend class ISingleton<PrimitiveBox>;

public:
	void Initialize(Fngine* engine, uint32_t numInstance)override;
private:
	PrimitiveBoxForGPU ConvertToGPUData(const PrimitiveBoxData& data)override;
	
	// テストのために仕方なく
private:
	// [ Resource ]
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	// [ Data一覧 ]
	Material* materialData_ = nullptr;
};

