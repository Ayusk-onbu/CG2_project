#pragma once
#include <wrl.h>
#include"Fngine.h"

template<typename T>
class Structured
{
public:
	Structured(Fngine* fngine) : p_fngine_(fngine) {}
public:
	// 初期化
	void Initialize(uint32_t numElements);

	// CPUからアクセス可能なバッファのポインタ
	T* GetMappedData()const { return mappedData_; }

	// GPU側のSRV Descriptor Handleを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU()const { return srvHandleGPU_; }

	// 要素数を取得
	uint32_t GetNumElements()const { return numElements_; }
private:
	Fngine* p_fngine_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource>resource_;
	T* mappedData_ = nullptr;
	uint32_t numElements_ = 0;

	// SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};
};

template<typename T>
void Structured<T>::Initialize(uint32_t numElements) {
	numElements_ = numElements;

	// リソースを作成
	resource_ = CreateBufferResource(
		p_fngine_->GetD3D12System().GetDevice().Get(),
		sizeof(T) * numElements_);

	// アドレスを取得
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));

	// SRVの作成
	srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc_.Buffer.FirstElement = 0;
	srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc_.Buffer.NumElements = numElements_;
	srvDesc_.Buffer.StructureByteStride = sizeof(T);

	srvHandleCPU_ = p_fngine_->GetSRV().GetCPUDescriptorHandle();
	srvHandleGPU_ = p_fngine_->GetSRV().GetGPUDescriptorHandle();

	p_fngine_->GetD3D12System().GetDevice()->CreateShaderResourceView(
		resource_.Get(), &srvDesc_, srvHandleCPU_
	);

	// 初期化
	std::memset(mappedData_, 0, sizeof(T) * numElements_);
}

