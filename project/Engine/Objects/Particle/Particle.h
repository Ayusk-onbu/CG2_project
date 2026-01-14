#pragma once
#include "ResourceFunc.h"
#include "TransformationMatrix.h"
#include "VertexData.h"
#include "Material.h"
#include "WorldTransform.h"

#include "Structured.h"
#include "ParticleEmitter.h"
#include "ParticleData.h"
#include "ForceField.h"

class Particle
{
public:
	// コンストラクタ
	Particle(Fngine* fngine);
public:
	void Initialize(uint32_t numInstance);
	void Update();
	void Draw();
	void DrawDebug();
	const std::list<std::unique_ptr<ParticleData>>& GetInfoList() const { return info_; }
	std::list<std::unique_ptr<ParticleData>>& GetInfoListNonConst() { return info_; }
	uint32_t GetNumMaxInstance() { return numMaxInstance_; }
	void AddParticle(std::unique_ptr<ParticleData>data);
	void AddParticleEmitter(std::string name);
	void AddPointForce(std::string name);
	void AddGravityForce(std::string name);
	void MakeEmitter();
	void MakeForceField();
public:
	std::list<std::unique_ptr<ParticleEmitter>>emitters_;
	std::list<std::unique_ptr<ForceField>>forceFields_;

	// インスタンス用バッファ
	std::unique_ptr < Structured<ParticleForGPU>> instancingBuffer_;

	// Resource
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	// 扱うためのData
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	// Fngine
	Fngine* p_fngine_ = nullptr;
	// 出す数
	uint32_t numMaxInstance_;
	// 位置情報
	std::list<std::unique_ptr<ParticleData>> info_;
	// Texture
	std::string textureName_;
	std::string input_name_buffer = "";
	// 色
	Vector4 color_{};
	// 高さ、横幅
	float height_ = 1.0f;
	float width_ = 1.0f;

};