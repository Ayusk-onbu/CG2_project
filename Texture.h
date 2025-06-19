#pragma once
#include <string>

#include "ShaderResourceView.h"
//#include "D3D12System.h"
#include "externals/DirectXTex/DirectXTex.h"


class Texture
{
public:
	void Initialize(D3D12System d3d12, const std::string& filePath,int num);
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() { return textureSrvHandleGPU_; }
private:
	/// <summary>
	/// DescriptorHandleを取得する関数(CPU)
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	/// <summary>
	/// DescriptorHandleを取得する関数(GPU)
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	void SetDesc(DXGI_FORMAT fmt, UINT mapping, D3D12_SRV_DIMENSION dimension, UINT mipLevel);
private:
	DirectX::ScratchImage mipImages_;
	//DirectX::TexMetadata metaData_;
	Microsoft::WRL::ComPtr <ID3D12Resource> textureResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
};

