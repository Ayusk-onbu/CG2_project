#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>

#include "D3D12System.h"

enum class ROOTTYPE {
	Normal,
	Structured,
	Skinning,
	CopyImage,
	SkyBox,
	PrimitiveBox
};

class RootSignature
{
public:
	/// <summary>
	/// 送りたい情報の設定、紐づけ
	/// </summary>
	/// <param name="d3d12"></param>
	/// <param name="type"></param>
	void CreateRootSignature(D3D12System& d3d12, ROOTTYPE type);

	Microsoft::WRL::ComPtr <ID3D12RootSignature>& GetRS() { return rootSignature_; }

private:
	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;
};

class RootSignatureBuilder {
public:
	/// <summary>
	/// ルート定数バッファ(CBV)の追加
	/// </summary>
	/// <param name="shaderRegister">レジスタの番号</param>
	/// <param name="visibility">何のShaderを使うか(VS..? PS..? ...etc)</param>
	void AddCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility);

	// ディスクリプタテーブル(SRV/UAVなど)の追加
	void AddSRVTable(UINT baseShaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);

	// 
	void AddUAVTable(UINT baseShaderRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);

	// 静的サンプラーの追加
	void AddStaticSampler(UINT shaderRegister, D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	// 最終的に RootSignature を生成して返す
	Microsoft::WRL::ComPtr<ID3D12RootSignature> Build(ID3D12Device* device);

private:
	std::vector<D3D12_ROOT_PARAMETER> parameters_;
	// DescriptorRangeのメモリ実体を保持するためのコンテナ (ポインタの破綻を防ぐ)
	std::vector<std::unique_ptr<D3D12_DESCRIPTOR_RANGE>> rangePool_;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers_;
};

