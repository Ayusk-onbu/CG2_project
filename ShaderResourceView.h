#pragma once
#include "DescriptorHeap.h"
#include "D3D12System.h"

class ShaderResourceView
{
public:
	static void InitializeHeap(D3D12System& d3d12);
	static void MakeDescriptorHeap(D3D12System& d3d12) { descriptorHeap_.CreateDescriptorHeap(d3d12.GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true); }
	static void SetSize(D3D12System d3d12) { descriptorSizeSRV_ = d3d12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }
	/// <summary>
	/// DescriptorHandleを取得する関数(CPU)
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle();
	/// <summary>
	/// DescriptorHandleを取得する関数(GPU)
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle();

public:
	static DescriptorHeap descriptorHeap_;
	static uint32_t descriptorSizeSRV_;
	static int descriptorIndex_;
};

using SRV = ShaderResourceView;

