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
	static void Initialize(Window window);
	/// <summary>
	/// 画面の幅を決める
	/// </summary>
	/// <param name="width 画面の幅"></param>
	static void SetWidth(LONG width);
	/// <summary>
	/// 画面の高さを決める
	/// </summary>
	/// <param name="height 画面の高さ"></param>
	static void SetHeight(LONG height);
	/// <summary>
	/// trueならSRGBを使う falseならSRGBを使わない
	/// </summary>
	static void SetFormat(bool IsSRGB);
	/// <summary>
	/// サンプリング数を決める
	/// </summary>
	/// <param name="num　サンプリング数"></param>
	static void SetSampleCount(int num);

	static void SetUseBufferCount(int num);

public:

	static void MakeResource();

	static Microsoft::WRL::ComPtr <ID3D12Resource> GetResource(int i) { return resources_[i]; }

	/*DXGI_SWAP_CHAIN_DESC1& GetDesc() { return swapChainDesc_; }

	Microsoft::WRL::ComPtr <IDXGISwapChain4> Get() { return swapChain_; }*/
public:
	// CDプレイヤー
	static Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_;
	// CD(情報を入れる場所)
	static DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;

	static Microsoft::WRL::ComPtr <ID3D12Resource> resources_[2];
};

