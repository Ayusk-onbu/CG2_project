#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "Window.h"

class SwapChain
{
public:
	/// <summary>
	/// SwapChainをどの様なものにするかの設定
	/// </summary>
	/// <param name="window"></param>
	void Initialize(Window window);
	/// <summary>
	/// 画面の幅を決める
	/// </summary>
	/// <param name="width 画面の幅"></param>
	void SetWidth(LONG width);
	/// <summary>
	/// 画面の高さを決める
	/// </summary>
	/// <param name="height 画面の高さ"></param>
	void SetHeight(LONG height);
	/// <summary>
	/// trueならSRGBを使う falseならSRGBを使わない
	/// </summary>
	void SetFormat(bool IsSRGB);
	/// <summary>
	/// サンプリング数を決める
	/// </summary>
	/// <param name="num　サンプリング数"></param>
	void SetSampleCount(int num);

	void SetUseBufferCount(int num);

	Microsoft::WRL::ComPtr <ID3D12Resource>& GetResource(int i) { return resources_[i]; }

	Microsoft::WRL::ComPtr <IDXGISwapChain4>& GetSwapChain() { return swapChain_; }

	DXGI_SWAP_CHAIN_DESC1& GetDesc() { return swapChainDesc_; }

public:

	void MakeResource();

	/*DXGI_SWAP_CHAIN_DESC1& GetDesc() { return swapChainDesc_; }

	Microsoft::WRL::ComPtr <IDXGISwapChain4> Get() { return swapChain_; }*/
private:
	// CDプレイヤー
	Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	// CD(情報を入れる場所)
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	Microsoft::WRL::ComPtr <ID3D12Resource> resources_[2];
};

