#include "ModelManager.h"
#include <sstream>
#include "Log.h"

std::unique_ptr<ModelManager>ModelManager::instance_ = nullptr;

void ModelManager::Initialize(Fngine* fngine) {
	pFngine_ = fngine;
}

std::string ModelManager::LoadObj(const std::string& filename, const std::string& directoryPath) {
	// model を生成
	std::unique_ptr<ModelObject>model = std::make_unique<ModelObject>();
	// Model 初期化 -この処理はObjファイルのみになってしまっている
	model->Initialize(pFngine_->GetD3D12System(),filename + ".obj", directoryPath);
	// Model Name を生成
	std::string modelName = filename;
	// 追加
	models_.emplace(modelName, std::move(model));
	// 返す
	return modelName;
}

ModelData& ModelManager::LoadModelData(const std::string& ID) {
	auto it = models_.find(ID);
	if (it == models_.end()) {
		Log::ViewFile("Not Found Model Data");
	}
	return it->second->GetModelData();
}