#pragma once
#include <wrl.h>
#include "ResourceFunc.h"
#include "TransformationMatrix.h"
#include "VertexData.h"
#include "Material.h"
#include "WorldTransform.h"
#include "Fngine.h"

class Particle
{
public:
	struct Info {
		WorldTransform worldTransform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
public:
	// コンストラクタ
	Particle(Fngine* fngine);
public:
	void Initialize(uint32_t numInstance);
	void Update();
	void Draw();
	void Create();
private:
	// Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	// 扱うためのData
	ParticleForGPU* instancingData_ = nullptr;
	VertexData* vertexData_;
	uint32_t* indexData_;
	Material* materialData_;
	// Fngine
	Fngine* p_fngine_ = nullptr;
	// 出す数
	uint32_t numMaxInstance_;
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;
	// 位置情報
	std::list<std::unique_ptr<Info>> info_;
	// Texture
	int textureHandle_ = 1;
	// 色
	Vector4 color_{};
	// 高さ、横幅
	float height_ = 100.0f;
	float width_ = 100.0f;
};