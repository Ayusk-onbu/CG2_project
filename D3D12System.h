#pragma once
#include <wrl.h>
#include <d3d12.h>

class D3D12System
{
public:
	Microsoft::WRL::ComPtr <ID3D12Device> GetDevice() { return device_; }
	/// <summary>
	/// D3D12Deviceの生成
	/// </summary>
	void SelectLevel();
private:
	Microsoft::WRL::ComPtr <ID3D12Device> device_ = nullptr;
};

