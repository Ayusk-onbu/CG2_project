#pragma once
#include "Log.h"
#include "RootSignature.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencil.h"
#include "DXC.h"

class Fngine;

enum class PIPELINETYPE {
	Graphics,// これは VS, PS, (GS, DS, HS etc...)
	Compute  // CS のみに活用
};

enum class PSOTYPE {
	Normal,
	Line,
	Skinning,
	CopyImage,
	SkyBox,
};

struct ShaderCompileSettings {
	// グラフィックス用のシェーダー
	std::wstring vsFilePath;
	std::wstring psFilePath;
	// ( GS, HS, DS も追加して良い )

	// コンピュート用シェーダー
	std::wstring csFilePath;

	// プロファイル
	const wchar_t* vsProfile = L"vs_6_0";
	const wchar_t* psProfile = L"ps_6_0";
	const wchar_t* csProfile = L"cs_6_0";
};

struct PSOKey {
	PIPELINETYPE pipelineType;// pipelineのType設定
	ROOTTYPE rootSignatureType;// rootSignatureのType設定
	PSOTYPE psoType;

	ShaderCompileSettings shaderCompileSettings;

	// ラスタライザ設定
	RasterizerSettings rasterizerSettings;

	DepthSettings depthSettings;
};

class PipelineStateObject
{
public:
	//void Initialize(Fngine* fngine, PSOTYPE type);

	void Initialize(Fngine* fngine, const PSOKey& key);

	void InitializeDirectly(
		Fngine* engine, 
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
		const D3D12_INPUT_ELEMENT_DESC* inputElement, UINT numInputElement,
		BLENDMODE startBlendMode,
		std::wstring vsPath, std::wstring psPath,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType,
		RasterizerSettings rasterizerSettings,
		DepthSettings depthSettings
		);

	void InitializeDirectlyCompute(
		Fngine* engine,
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,
		const D3D12_INPUT_ELEMENT_DESC* inputElement, UINT numInputElement,
		std::wstring csPath
	);

	void Compile(
		//CompilerするShaderファイルへのパス
		const std::wstring& vsFilePath,
		//Compilerに使用するProfile
		const wchar_t* vsProfile,
		//CompilerするShaderファイルへのパス
		const std::wstring& psFilePath,
		//Compilerに使用するProfile
		const wchar_t* psProfile);

	Microsoft::WRL::ComPtr < IDxcBlob> CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを三つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	/// <summary>
	/// PSOのDescに情報をまとめる
	/// </summary>
	void MargeDesc();

	// --------------------------------
	// Set 
	// --------------------------------

	void SetDesc(D3D12System& d3d12, const PSOKey& key);

	RootSignature GetRootSignature() { return rootSignature_; }
	InputLayout GetInputLayout() { return inputLayoutDesc_; }
	BlendState GetBlendState() { return blendState_; }
	void SetBlendState(BLENDMODE blendMode) { blendMode_ = blendMode; }
	RasterizerState  GetRasterizer() { return rasterizer_; }
	Microsoft::WRL::ComPtr<IDxcBlob>& GetVSB() { return vertexShaderBlob_; }
	Microsoft::WRL::ComPtr<IDxcBlob>& GetPSB() { return pixelShaderBlob_; }
	Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetGPS();
	Microsoft::WRL::ComPtr <ID3D12PipelineState>& GetCPS() { return computePipelineState_; }

private:
	DXC* dxc_;
	RootSignature rootSignature_;
	InputLayout inputLayoutDesc_;
	BlendState blendState_;
	RasterizerState rasterizer_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	DepthStencil depthStencil_;

	// ブレンドモード
	BLENDMODE blendMode_ = BLENDMODE::AlphaBlend;

	PIPELINETYPE pipelineType_ = PIPELINETYPE::Graphics;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Add = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Sub = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Mul = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState_Scr = nullptr;

	//ComputePipeline
	Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob_;
	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr <ID3D12PipelineState> computePipelineState_ = nullptr;
};

using PSO = PipelineStateObject;

class PSOBuilder {
public:
	PSOBuilder(Fngine* fngine, const std::string& psoName);

	// 各種設定をチェーンで繋ぐための関数群
	PSOBuilder& SetPipelineType(const std::string& typeName);
	PSOBuilder& SetShaders(const std::wstring& vsPath, const std::wstring& psPath);
	PSOBuilder& SetComputeShader(const std::wstring& csPath);
	PSOBuilder& SetInputLayout(const D3D12_INPUT_ELEMENT_DESC* pElements, UINT numElements);
	PSOBuilder& SetBlendMode(BLENDMODE mode);
	PSOBuilder& SetTopologyType(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& type);
	PSOBuilder& SetRasterizerSettings(const RasterizerSettings& settings);
	PSOBuilder& SetDepthSettings(const DepthSettings& settings);

	// RootSignatureの設定もここにパススルーする
	PSOBuilder& AddCBV(UINT reg, D3D12_SHADER_VISIBILITY vis) { rsBuilder_.AddCBV(reg, vis); return *this; }
	PSOBuilder& AddSRVTable(UINT reg, UINT num, D3D12_SHADER_VISIBILITY vis) { rsBuilder_.AddSRVTable(reg, num, vis); return *this; }
	PSOBuilder& AddUAVTable(UINT reg, UINT num, D3D12_SHADER_VISIBILITY vis) { rsBuilder_.AddUAVTable(reg, num, vis); return *this; }
	PSOBuilder& AddStaticSampler(UINT reg) { rsBuilder_.AddStaticSampler(reg); return *this; }

	// 最後にこれを呼ぶとManagerに登録される
	void Build();

private:
	Fngine* fngine_;
	std::string name_;

	// 詰め込むデータ群 (現状のPSOKeyに入っていたようなもの)
	std::string pipelineType_;
	std::wstring vsPath_, psPath_, csPath_;
	const D3D12_INPUT_ELEMENT_DESC* pInputElements_ = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC>inputElements_;
	UINT numInputElements_ = 0;
	BLENDMODE blendMode_ = BLENDMODE::AlphaBlend;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_;
	RasterizerSettings rasterizerSettings_;
	DepthSettings depthSettings_;

	RootSignatureBuilder rsBuilder_;
};