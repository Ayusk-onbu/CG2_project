#pragma once
#include "PipelineStateObject.h"
#include <unordered_map>

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

	void LoadAllPSOsFromDirectory(const std::string& directoryPath);

	void LoadPSOsFromJson(const std::string& filepath, const std::string& psoName);

	void RegisterPSO(const std::string& name, PSO&& newPSO) {
		// 保管庫（マップ）に、名前をキーにして「引っ越し（move）」しながら登録する
		PSOs_.emplace(name, std::move(newPSO));
	}

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

