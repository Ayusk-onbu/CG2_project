#include "RenderTargetView.h"
#include "SwapChain.h"
#include "OffScreenRendering.h"

void RenderTargetView::Initialize(D3D12System* d3d12, DXGI_FORMAT fmt, D3D12_RTV_DIMENSION dimension) {
	descriptorHeap_.CreateDescriptorHeap(d3d12->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	MakeDesc(fmt, dimension);
	SetHandle();
	MakeHandle(d3d12->GetDevice());
}

void RenderTargetView::MakeDesc(DXGI_FORMAT fmt, D3D12_RTV_DIMENSION dimension) {
	desc_.Format = fmt;//出力結果をSRGBに変換して書き込む
	desc_.ViewDimension = dimension;//２Dテクスチャッとして書き込む
}

void RenderTargetView::SetHandle() {
	startHandle_ = descriptorHeap_.descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
}

void RenderTargetView::MakeHandle(Microsoft::WRL::ComPtr <ID3D12Device> device) {
	//まず1つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	handles_[0] = startHandle_;
    device->CreateRenderTargetView(SwapChain::GetResource(0).Get(), &desc_, handles_[0]);
	//2二つ目のディスクリプタ班どりを作る（自力で）
	handles_[1].ptr = handles_[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device->CreateRenderTargetView(SwapChain::GetResource(1).Get(), &desc_, handles_[1]);
}

void OffRTV::Initialize(D3D12System* d3d12, Microsoft::WRL::ComPtr <ID3D12Resource> resource,DXGI_FORMAT fmt, D3D12_RTV_DIMENSION dimension) {
	descriptorHeap_.CreateDescriptorHeap(d3d12->GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	MakeDesc(fmt, dimension);
	SetHandle();
	MakeHandle(d3d12->GetDevice(),resource);
}

void OffRTV::MakeDesc(DXGI_FORMAT fmt, D3D12_RTV_DIMENSION dimension) {
	desc_.Format = fmt;//出力結果をSRGBに変換して書き込む
	desc_.ViewDimension = dimension;//２Dテクスチャッとして書き込む
}

void OffRTV::SetHandle() {
	startHandle_ = descriptorHeap_.descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
}

void OffRTV::MakeHandle(Microsoft::WRL::ComPtr <ID3D12Device> device,Microsoft::WRL::ComPtr <ID3D12Resource> resource) {
	handle_ = startHandle_;
	device->CreateRenderTargetView(resource.Get(), &desc_, handle_);
}