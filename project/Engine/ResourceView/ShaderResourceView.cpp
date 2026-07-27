#include "ShaderResourceView.h"
#include <cassert>

SRVAllocation ShaderResourceView::Allocate() {
    // 限界値を超えないかチェック（重要）
    assert(descriptorIndex_ < (int)kMaxSRVCount_ && "SRV Descriptor Heap is full!");

    SRVAllocation alloc;
    alloc.index = descriptorIndex_;

    // CPUハンドルの計算
    alloc.cpu = descriptorHeap_.GetHeap()->GetCPUDescriptorHandleForHeapStart();
    alloc.cpu.ptr += descriptorSizeSRV_ * descriptorIndex_;

    // GPUハンドルの計算
    alloc.gpu = descriptorHeap_.GetHeap()->GetGPUDescriptorHandleForHeapStart();
    alloc.gpu.ptr += descriptorSizeSRV_ * descriptorIndex_;

    // 次の割り当てに向けてインデックスを進める
    descriptorIndex_++;

    return alloc;
}

void ShaderResourceView::InitializeHeap(D3D12System& d3d12) {
	MakeDescriptorHeap(d3d12);
	SetSize(d3d12);
}