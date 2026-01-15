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
	// 初期化処理
	void Initialize(uint32_t numInstance, const std::string& name, const std::string& textureName = "circle");
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// デバッグ描画処理
	void DrawDebug();
	// ImGui
	void ImGui();
public:
	// Add関係の処理
	void AddParticle(std::unique_ptr<ParticleData>data);
	void AddParticleEmitter(std::string name);
	void AddPointForce(std::string name);
	void AddGravityForce(std::string name);
public:
	// Get関係の処理
	const std::list<std::unique_ptr<ParticleData>>& GetInfoList() const { return info_; }
	std::list<std::unique_ptr<ParticleData>>& GetInfoListNonConst() { return info_; }
	uint32_t GetNumMaxInstance() { return numMaxInstance_; }
public:
	// Set関係の処理
	// [ 使うTextureのName ]
	void SetTextureName(const std::string& textureName) { textureName_ = textureName; }
	// [ ローカル原点の位置 ]
	void SetPosition(const Vector3& pos) { transform_.transform_.translation_ = pos; }
private:
	// パーティクルを保存
	void SaveParticle();

	void LoadParticle();
private:
	// *** 外部情報 ***
	// [ エンジンのポインタ ]
	Fngine* p_fngine_ = nullptr;
private:
	// *** パーティクルに関する情報 ***
	// [ エミッター ]
	std::list<std::unique_ptr<ParticleEmitter>>emitters_;

	// [ 重力場 ]
	std::list<std::unique_ptr<ForceField>>forceFields_;

	// [ 出せる最大数 ]
	uint32_t numMaxInstance_;

	// [ パーティクルの情報 ]
	std::list<std::unique_ptr<ParticleData>> info_;

	// [ Texture ]
	std::string textureName_;
	std::string input_name_buffer = "";

	// [ 色 ]
	Vector4 color_{};

	// [ 高さ、横幅 ]
	float height_ = 1.0f;
	float width_ = 1.0f;

	// [ ローカル原点の位置 ]
	WorldTransform transform_;
public:
	// [ パーティクルの名前 ]
	std::string name_;
private:
	// *** Structured etc のそちらの情報 ***
	// [ インスタンス用バッファ ]
	std::unique_ptr<Structured<ParticleForGPU>> instancingBuffer_;

	// [ Resource ]
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	
	// [ View ]
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// [ Data一覧 ]
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
};