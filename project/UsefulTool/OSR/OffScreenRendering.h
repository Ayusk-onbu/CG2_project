#pragma once
#include <wrl.h>
#include "D3D12System.h"
#include "DescriptorHeap.h"
#include "ShaderResourceView.h"
#include "DepthStencil.h"
#include "PipelineStateObject.h"
#include "TheOrderCommand.h"

class OffRTV
{
public:

	void Initialize(D3D12System* d3d12, Microsoft::WRL::ComPtr <ID3D12Resource> resource,DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_RTV_DIMENSION dimension = D3D12_RTV_DIMENSION_TEXTURE2D);

	void MakeDesc(DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D12_RTV_DIMENSION dimension = D3D12_RTV_DIMENSION_TEXTURE2D);

	void MakeHandle(Microsoft::WRL::ComPtr <ID3D12Device> device, Microsoft::WRL::ComPtr <ID3D12Resource> resource);

	D3D12_CPU_DESCRIPTOR_HANDLE& GetHandle() { return handle_; }

private:
	DescriptorHeap descriptorHeap_;
	D3D12_RENDER_TARGET_VIEW_DESC desc_;
	D3D12_CPU_DESCRIPTOR_HANDLE startHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE handle_;

};

class OffScreenRendering
{
public:

	void Initialize(D3D12System& d3d12, float width,float height,
		DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 
		D3D12_RTV_DIMENSION dimension = D3D12_RTV_DIMENSION_TEXTURE2D
		);
	void Begin(TheOrderCommand &command);
	void End(TheOrderCommand& command);
	D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU() { return srvAllocation_.gpu; }
	Microsoft::WRL::ComPtr <ID3D12Resource> GetResource() {return offScreenTexture_;}
	void ChangeDSVHandleType(TheOrderCommand& command,DSV_HANDLE_TYPE type);

	Microsoft::WRL::ComPtr <ID3D12Resource>GetDSVResource() { return dsv_.GetResource(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVDepthHandleGPU() { return dsv_.GetSRVHandleGPU(); }
private:
	
private:
	Microsoft::WRL::ComPtr <ID3D12Resource> offScreenTexture_;
	OffRTV offRTV_;
	DSV dsv_;
	PSO pso_;
	SRVAllocation srvAllocation_;
};