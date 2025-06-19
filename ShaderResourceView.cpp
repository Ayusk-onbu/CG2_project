#include "ShaderResourceView.h"

DescriptorHeap ShaderResourceView::descriptorHeap_;
uint32_t ShaderResourceView::descriptorSizeSRV_;

void ShaderResourceView::InitializeHeap(D3D12System d3d12) {
	MakeDescriptorHeap(d3d12);
	SetSize(d3d12);
}