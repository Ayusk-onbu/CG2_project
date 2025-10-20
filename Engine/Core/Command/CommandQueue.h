#pragma once
#include <wrl.h>
#include <d3d12.h>

class CommandQueue
{
public:

	void SetDescD();

	void SetDescC();

	Microsoft::WRL::ComPtr <ID3D12CommandQueue>& GetQueue() { return commandQueue_; }

	D3D12_COMMAND_QUEUE_DESC& GetDesc() { return commandQueueDesc_; }

private:
	//コマンドの生成
	Microsoft::WRL::ComPtr <ID3D12CommandQueue> commandQueue_;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_;
};

