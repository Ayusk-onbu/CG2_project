#include "PipelineStateObjectManager.h"

std::unique_ptr<PipelineStateObjectManager> PipelineStateObjectManager::instance_ = nullptr;

void PipelineStateObjectManager::Initialize(Fngine* fngine) { 
	p_fngine_ = fngine;
}

void PipelineStateObjectManager::CreateNewPSO(const PSOKey& key,const std::string& name) {
	PSO newPSO;
	newPSO.Initialize(
		p_fngine_,
		key.pipelineType,
		key.psoType,
		key.rootSignatureType,
		key.rasterizerSettings,
		key.depthFlag,
		key.shaderCompileSettings.vsFilePath,
		key.shaderCompileSettings.vsProfile,
		key.shaderCompileSettings.psFilePath,
		key.shaderCompileSettings.psProfile
	);
	PSOs_.emplace(name, std::move(newPSO));
}

PSO& PipelineStateObjectManager::GetPSO(const std::string& name) {
	auto it = PSOs_.find(name);
	if (it == PSOs_.end()) {
		Log::ViewFile("Not find your want File");
	}
	return it->second;
}