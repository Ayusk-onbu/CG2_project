#pragma once
#include <wrl.h>
#include <d3d12.h>

class D3D12System
{
public:
	Microsoft::WRL::ComPtr <ID3D12Device> GetDevice() { return device_; }
	void SelectLevel();
private:
	Microsoft::WRL::ComPtr <ID3D12Device> device_ = nullptr;
};

