#pragma once
#include "PipelineStateObject.h"
#include <unordered_map>

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

	// Depthの設定
	bool depthFlag;
};

class PipelineStateObjectManager
{
public:
	static PipelineStateObjectManager* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<PipelineStateObjectManager>();
		}
		return instance_.get();
	}
	static void ReleaseInstance() { instance_.reset(); }
public:
	void Initialize(Fngine* fngine);

	/// <summary>
	/// PSOを作成する関数
	/// </summary>
	/// <param name="key　セッティングの内容"></param>
	/// <param name="name　キー"></param>
	void CreateNewPSO(const PSOKey& key, const std::string& name);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="name"></param>
	PSO& GetPSO(const std::string& name);

	/// <summary>
	/// 主にBlendModeのImGuiである
	/// </summary>
	void ImGui();
private:
	static std::unique_ptr<PipelineStateObjectManager>instance_;

	// これここなのかな？
	//void CreateGraphicsPipelineState();
	//void CreateComputePipelineState();

	Fngine* p_fngine_ = nullptr;
	std::unordered_map<std::string, PSO>PSOs_;
};

using PSOManager = PipelineStateObjectManager;

