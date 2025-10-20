#include "ShaderResourceView.h"

void ShaderResourceView::InitializeHeap(D3D12System& d3d12) {
	MakeDescriptorHeap(d3d12);
	SetSize(d3d12);
}

// DescriptorHandleを取得する関数(CPU)
D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourceView::GetCPUDescriptorHandle() {
	descriptorIndex_ += 1;
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_.GetHeap().Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += descriptorSizeSRV_ * descriptorIndex_;
	return handleCPU;
}
// DescriptorHandleを取得する関数(GPU)
D3D12_GPU_DESCRIPTOR_HANDLE ShaderResourceView::GetGPUDescriptorHandle() {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_.GetHeap().Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += descriptorSizeSRV_ * descriptorIndex_;
	return handleGPU;
}