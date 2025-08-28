#include "SwapChain.h"
#include <cassert>

void SwapChain::Initialize(Window window) {
	swapChain_ = nullptr;
	swapChainDesc_ = {};
	SetWidth(window.GetWidth());
	SetHeight(window.GetHeight());
	SetFormat(false);
	SetSampleCount(1);
	// バッファとは途中式が書いてある紙のようなもの（一時的な保存場所）
	// 一個目のバッファを描画用にするという処理
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SetUseBufferCount(2);
	//モニタにうつしたら、中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	
}

void SwapChain::SetWidth(LONG width) {
	swapChainDesc_.Width = width;
}

void SwapChain::SetHeight(LONG height) {
	swapChainDesc_.Height = height;
}

void SwapChain::SetFormat(bool IsSRGB) {
	swapChainDesc_.Format = IsSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
}

void SwapChain::SetSampleCount(int num) {
	swapChainDesc_.SampleDesc.Count = num;
}

void SwapChain::SetUseBufferCount(int num) {
	swapChainDesc_.BufferCount = num;
}

void SwapChain::MakeResource() {
	HRESULT hr;
	hr = SwapChain::swapChain_->GetBuffer(0, IID_PPV_ARGS(&resources_[0]));
	//上手く取得出来なければ起動できない
	assert(SUCCEEDED(hr));
	hr = SwapChain::swapChain_->GetBuffer(1, IID_PPV_ARGS(&resources_[1]));
	assert(SUCCEEDED(hr));
}