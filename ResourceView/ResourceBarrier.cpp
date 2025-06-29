#include "ResourceBarrier.h"


void ResourceBarrier::SetBarrier(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
	Microsoft::WRL::ComPtr <ID3D12Resource> resource,
	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = resource.Get();
	SetTransition(commandList,before,after);
}

void ResourceBarrier::Transition(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	//遷移前の（現在）のResourceState
	barrier_.Transition.StateBefore = before;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = after;
}

void ResourceBarrier::SetTransition(Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList,
	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
	Transition(before, after);
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier_);
}