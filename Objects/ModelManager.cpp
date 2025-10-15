#include "ModelManager.h"
#include <sstream>
#include "Log.h"

//void ModelManager::Initialize(Fngine* fngine) {
//	pFngine_ = fngine;
//}
//
//int ModelManager::LoadObj(const std::string& filename, const std::string& directoryPath = "resources") {
//	std::unique_ptr<ModelObject>model = std::make_unique<ModelObject>();
//	model->Initialize(pFngine_->GetD3D12System(),filename + ".obj", directoryPath);
//	ModelID modelId = { modelCount_,filename };
//	models_[modelId].push_back(model);
//}
//
//void ModelManager::DrawModel(ModelID modelId) {
//
//}