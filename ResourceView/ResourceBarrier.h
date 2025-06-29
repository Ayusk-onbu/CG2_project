#pragma once
#include <d3d12.h>
#include "wrl.h"

class ResourceBarrier
{
public:

	// まとめたやつ
	void SetBarrier(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
		Microsoft::WRL::ComPtr <ID3D12Resource> resource,
		D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	// 遷移前と遷移後を設定したらバリアを貼るシステム
	void Transition(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	void SetTransition(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
							D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:
	D3D12_RESOURCE_BARRIER barrier_;
};

