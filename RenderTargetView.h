#pragma once
#include<wrl.h>
#include "DescriptorHeap.h"
#include "D3D12System.h"

class RenderTargetView
{
public:

	void Initialize(D3D12System* d3d12, DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_RTV_DIMENSION dimension = D3D12_RTV_DIMENSION_TEXTURE2D);

	void MakeDesc(DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_RTV_DIMENSION dimension = D3D12_RTV_DIMENSION_TEXTURE2D);

	void SetHandle();

	void MakeHandle(Microsoft::WRL::ComPtr <ID3D12Device> device);

	D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle(int i) { return handles_[i]; }

private:
	DescriptorHeap descriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC desc_;
	D3D12_CPU_DESCRIPTOR_HANDLE startHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE handles_[2];
};

