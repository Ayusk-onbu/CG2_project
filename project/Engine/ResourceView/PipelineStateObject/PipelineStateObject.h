#pragma once
#include "Log.h"
#include "RootSignature.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencil.h"
#include "DXC.h"


enum class PSOTYPE {
	Normal,
	OffScreen,
	Line
};

class PipelineStateObject
{
public:
	void Initialize(D3D12System& d3d12, PSOTYPE type);

	void InitializeDescs(D3D12System& d3d12,PSOTYPE type);

	void Compile();

	Microsoft::WRL::ComPtr < IDxcBlob> CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを三つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	void MargeDesc();

	void SetDesc(D3D12System& d3d12, PSOTYPE type);

	DXC GetDXC() { return dxc_; }
	RootSignature GetRootSignature() { return rootSignature_; }
	InputLayout GetInputLayout() { return inputLayoutDesc_; }
	BlendState GetBlendState() { return blendState_; }
	void SetBlendState(BLENDMODE blendMode) { blendMode_ = blendMode; }
	RasterizerState  GetRasterizer() { return rasterizer_; }
	Microsoft::WRL::ComPtr<IDxcBlob>& GetVSB() { return vertexShaderBlob_; }
	Microsoft::WRL::ComPtr<IDxcBlob>& GetPSB() { return pixelShaderBlob_; }
	Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGPS();

private:
	DXC dxc_;
	RootSignature rootSignature_;
	InputLayout inputLayoutDesc_;
	BlendState blendState_;
	RasterizerState rasterizer_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	DepthStencil depthStencil_;

	// ブレンドモード
	BLENDMODE blendMode_ = BLENDMODE::AlphaBlend;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Add = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Sub = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Mul = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Scr = nullptr;
};

using PSO = PipelineStateObject;

