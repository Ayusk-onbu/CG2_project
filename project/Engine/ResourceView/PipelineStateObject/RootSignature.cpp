#include "RootSignature.h"
#include "Log.h"
#include <cassert>

void RootSignature::CreateRootSignature(D3D12System& d3d12, ROOTTYPE type) {
	if (type == ROOTTYPE::Normal) {
		// 送りたい情報を設定。GPUに分かる形に変え（シリアライズでバイナリという形に変える）、
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
		descriptionRootSignature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// SRVの設定で必ず定義しなくてはいけないモノっぽい
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		// 何で始まるのか　今回はt0からなので0
		descriptorRange[0].BaseShaderRegister = 0;
		// SRVのテーブルは何個あるのか　今回はTextureのみなので1
		descriptorRange[0].NumDescriptors = 1;
		// デスクリプタの数はいくつなのか 今回はTextureなのでSRVだよね
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		// デーブル内でのオフセット（どこから開始するのか）
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//RootParameterの作成。複数設定できるので配列(CBufferの数に応じて増やす)
		D3D12_ROOT_PARAMETER rootParameters[7] = {};

		// Pixel Shader のConstantBuffer<>のb0に設定しているやつについて
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)とバインド

		/*SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		*
		*  rootParameters[0] の　0 は上記（Drawの際に設定する）の 0と対応している
		* なのでもしこちらでrootParameters[x]のxをかえるならDrawでも変えないとエラーを吐く
		* 
		*/
		

		// Vertex Shader のConstantBuffer<>のb0に設定しているやつについて
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//Use VertexShader
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)

		// Pixel Shader のDesciptorTable(SRV)のt0に設定しているやつについて
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Use DescriptorTable
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShader Use
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

		// Pixel Shader のConstantBuffer<>のb1に設定しているやつについて
		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号１(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb2に設定しているやつについて(CameraPosition)
		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[4].Descriptor.ShaderRegister = 2;//レジスタ番号2(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb3に設定しているやつについて(CameraPosition)
		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[5].Descriptor.ShaderRegister = 3;//レジスタ番号3(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb4に設定しているやつについて(CameraPosition)
		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[6].Descriptor.ShaderRegister = 4;//レジスタ番号3(hlslのやつ)とバインド

		descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

		//Samplerのせってい(Textureの読み取り設定)
		// SamplerState gSampler : register(s0);と対応している
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
	else if (type == ROOTTYPE::Structured) {
		// 送りたい情報を設定。GPUに分かる形に変え（シリアライズでバイナリという形に変える）、
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
		descriptionRootSignature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
		// StructuredBuffer t0 なので 0
		descriptorRangeForInstancing[0].BaseShaderRegister = 0;
		// SRVは一個しかない
		descriptorRangeForInstancing[0].NumDescriptors = 1;
		// SRV(StructuredBuffer)なので
		descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		// 開始の位置
		descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//RootParameterの作成。複数設定できるので配列(CBufferの数に応じて増やす)
		D3D12_ROOT_PARAMETER rootParameters[3] = {};

		// PixelのCBVのb0はMaterial
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)とバインド

		// 今回StructuredBuffer（SRV）なので　DescriptorTable
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		// 使うShaderの種類
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		// StructuredBufferの t0 なので 0
		rootParameters[1].Descriptor.ShaderRegister = 0;
		rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
		rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

		// PixelShaderのTexture2D（SRV）なのでDescriptorTable
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Use DescriptorTable
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShader Use
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数

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
	else if (type == ROOTTYPE::Skinning){
		// 送りたい情報を設定。GPUに分かる形に変え（シリアライズでバイナリという形に変える）、
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
		descriptionRootSignature.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// SRVの設定で必ず定義しなくてはいけないモノっぽい
		D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
		// 何で始まるのか　今回はt0からなので0
		descriptorRange[0].BaseShaderRegister = 0;
		// SRVのテーブルは何個あるのか　今回はTextureのみなので1
		descriptorRange[0].NumDescriptors = 1;
		// デスクリプタの数はいくつなのか 今回はTextureなのでSRVだよね
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		// デーブル内でのオフセット（どこから開始するのか）
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//RootParameterの作成。複数設定できるので配列(CBufferの数に応じて増やす)
		D3D12_ROOT_PARAMETER rootParameters[8] = {};

		// Pixel Shader のConstantBuffer<>のb0に設定しているやつについて
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)とバインド

		/*SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		*
		*  rootParameters[0] の　0 は上記（Drawの際に設定する）の 0と対応している
		* なのでもしこちらでrootParameters[x]のxをかえるならDrawでも変えないとエラーを吐く
		*
		*/


		// Vertex Shader のConstantBuffer<>のb0に設定しているやつについて
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//Use VertexShader
		rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号０(hlslのやつ)

		// Pixel Shader のDesciptorTable(SRV)のt0に設定しているやつについて
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Use DescriptorTable
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShader Use
		rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
		rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

		// Pixel Shader のConstantBuffer<>のb1に設定しているやつについて
		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号１(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb2に設定しているやつについて(CameraPosition)
		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[4].Descriptor.ShaderRegister = 2;//レジスタ番号2(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb3に設定しているやつについて(CameraPosition)
		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[5].Descriptor.ShaderRegister = 3;//レジスタ番号3(hlslのやつ)とバインド

		// Pixel Shader のConstantBuffer<>のb4に設定しているやつについて(CameraPosition)
		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//Use CBV
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Use PixelShader
		rootParameters[6].Descriptor.ShaderRegister = 4;//レジスタ番号3(hlslのやつ)とバインド

		// 今回StructuredBuffer（SRV）なので　DescriptorTable
		rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		// 使うShaderの種類
		rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		// StructuredBufferの t0 なので 0
		rootParameters[7].Descriptor.ShaderRegister = 0;
		rootParameters[7].DescriptorTable.pDescriptorRanges = descriptorRange;
		rootParameters[7].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

		descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
		descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

		//Samplerのせってい(Textureの読み取り設定)
		// SamplerState gSampler : register(s0);と対応している
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