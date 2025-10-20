#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include "Window.h"
#include "SwapChain.h"
#include <d3d12.h>

class DXGI
{
public:
	/// <summary>
	/// GPUのインフラ整備士を雇う
	/// </summary>
	void RecruitEngineer();

	void AssignTaskToEngineer(Microsoft::WRL::ComPtr <ID3D12CommandQueue>& commandQueue,const Window& window,SwapChain& swapChain);

public:
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory_;
};

