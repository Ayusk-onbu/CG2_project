#pragma once
#include "DescriptorHeap.h"
#include "D3D12System.h"

struct SRVAllocation {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu;
	uint32_t index;

	// 有効な割り当てかどうかを判定する便利関数
	bool IsValid() const {
		return cpu.ptr != 0;
	}
};

class ShaderResourceView
{
public:
	SRVAllocation Allocate();

	void InitializeHeap(D3D12System& d3d12);
	void MakeDescriptorHeap(D3D12System& d3d12) { descriptorHeap_.CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true); }
	void SetSize(D3D12System d3d12) { descriptorSizeSRV_ = d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }
	DescriptorHeap& GetDescriptorHeap() { return descriptorHeap_; }

private:
	DescriptorHeap descriptorHeap_;
	uint32_t descriptorSizeSRV_;
	int descriptorIndex_;
	const uint32_t kMaxSRVCount_ = 512;
};

using SRV = ShaderResourceView;

