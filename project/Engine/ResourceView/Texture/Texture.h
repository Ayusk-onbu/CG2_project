#pragma once
#include <string>

#include "ShaderResourceView.h"
//#include "D3D12System.h"
#include "externals/DirectXTex/DirectXTex.h"


class Texture
{
public:
	Texture() = default;
	Texture(const Texture&) = delete;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	Texture& operator=(const Texture&) = delete;
public:
	void Initialize(D3D12System& d3d12, SRV& srv,const std::string& filePath,int num);
	D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU() { return textureSrvHandleGPU_; }
private:
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	void UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	void SetDesc(DXGI_FORMAT fmt, UINT mapping, D3D12_SRV_DIMENSION dimension, UINT mipLevel);
private:
	// 下のImageいる？	
	DirectX::ScratchImage mipImages_;
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
};
