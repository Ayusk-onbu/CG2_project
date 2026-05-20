#include "ModelManager.h"
#include <sstream>
#include "Log.h"
#include "Trigonometric.h"

std::unique_ptr<ModelManager>ModelManager::instance_ = nullptr;

void ModelManager::Initialize(Fngine* fngine) {
	pFngine_ = fngine;
}

std::string ModelManager::LoadObj(const std::string& filename, const std::string& directoryPath, LoadFileType type) {
	// model を生成
	std::unique_ptr<ModelObject>model = std::make_unique<ModelObject>();
	// Model 初期化 -この処理はObjファイルのみになってしまっている
	model->Initialize(pFngine_->GetD3D12System(),filename, directoryPath,type);
	// Model Name を生成
	std::string modelName = filename;
	// ファイルの名前から.以降の拡張子を削除
	remove_extension_in_place(modelName);
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

ObjectData& ModelManager::LoadObjectData(const std::string& ID) {
	auto it = objects_.find(ID);
	if (it == objects_.end()) {
		Log::ViewFile("Not Found Model Data");
		return *objects_.begin()->second;
	}
	return *it->second;
}

void ModelManager::AddObject(const std::string& ID, const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices) {
	auto it = objects_.find(ID);
	if (it == objects_.end()) {
		Log::View("The : " + ID + "Object -> Not already Exists");
		std::unique_ptr<ObjectData>object = std::make_unique<ObjectData>();
		object->MakeObjectData(pFngine_, vertices, indices);
		objects_.emplace(ID, std::move(object));
	}
	else {
		Log::View("The : " + ID + "Object -> Already Exists");
		return;
	}
}