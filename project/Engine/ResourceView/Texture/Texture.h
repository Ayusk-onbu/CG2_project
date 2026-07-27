#pragma once
#include <string>
#include "ShaderResourceView.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "Structures.h"

class Texture
{
public:
	Texture() = default;
	Texture(const Texture&) = delete;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	Texture& operator=(const Texture&) = delete;
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> Initialize(D3D12System& d3d12, const std::string& filePath,int num, ID3D12GraphicsCommandList* commandList);
	D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU() { return srvAllocation_.gpu; }
	uint32_t GetSrvIndex()const { return srvAllocation_.index; }
	Vector2 GetSize()const { return textureSize_; }
	/// <summary>
	/// CPU側でテクスチャのピクセル（R成分）をサンプリングする
	/// </summary>
	/// <param name="u">UV座標のU (0.0 ～ 1.0)</param>
	/// <param name="v">UV座標のV (0.0 ～ 1.0)</param>
	/// <returns>0.0 ～ 1.0 のノイズ値</returns>
	float SampleNoiseCPU(float u, float v) const;
private:
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	[[nodiscard]]
	Microsoft::WRL::ComPtr <ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
private:
	// 下のImageいる？	
	DirectX::ScratchImage mipImages_;
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;
	
	Vector2 textureSize_; 
	SRVAllocation srvAllocation_;
};
