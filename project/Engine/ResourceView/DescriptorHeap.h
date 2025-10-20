#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>

class DescriptorHeap
{
public:
	void CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible); 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetHeap() { return descriptorHeap_; }
public:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
};

