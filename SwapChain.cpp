#include "SwapChain.h"

Microsoft::WRL::ComPtr <IDXGISwapChain4> SwapChain::swapChain_;
DXGI_SWAP_CHAIN_DESC1 SwapChain::swapChainDesc_;

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