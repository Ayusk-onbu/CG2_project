#include "RootSignature.h"
#include "Log.h"
#include <cassert>

void RootSignature::CreateRootSignature(D3D12System& d3d12, ROOTTYPE type) {
	if (type == ROOTTYPE::Normal) {
		// 送りたい情報を設定。GPUに分かる形に変え（シリアライズでバイナリという形に変える）、
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
		descriptionRootSignature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		//RootParameterの作成。複数設定できるので配列(CBufferの数に応じて増やす)
		D3D12_ROOT_PARAMETER rootParameters[4] = {};

		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		descriptorRange[0].BaseShaderRegister = 0;
		descriptorRange[0].NumDescriptors = 1;
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)とバインド

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//Use VertexShader
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Use DescriptorTable
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShader Use
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号１(hlslのやつ)とバインド

		descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

		//Samplerのせってい(Textureの読み取り設定)
		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		descriptionRootSignature.pStaticSamplers = staticSamplers;
		descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

		//シリアライズしてバイナりにする
		ID3DBlob* signatureBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		HRESULT hr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			Log::View(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}
		//バイナリをもとに生成
		//Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature = nullptr;
		hr = d3d12.GetDevice()->CreateRootSignature(0,
			signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature_));
		assert(SUCCEEDED(hr));
	}
}