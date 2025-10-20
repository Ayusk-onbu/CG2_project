#pragma once
#include <wrl.h>
#include <d3d12.h>

class CommandList
{
public:
	
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator>& GetAllocator() { return commandAllocator_; }

	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetList() { return commandList_; }

private:
	Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator_;

	Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;
};

