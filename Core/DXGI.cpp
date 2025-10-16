#include "DXGI.h"
#include <cassert>

void DXGI::RecruitEngineer() {
	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
}

void DXGI::AssignTaskToEngineer(Microsoft::WRL::ComPtr <ID3D12CommandQueue>& commandQueue, const Window& window, SwapChain& swapChain) {
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue.Get(), window.GetHwnd(), &swapChain.GetDesc(), nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetSwapChain().GetAddressOf()));
	assert(SUCCEEDED(hr));
}