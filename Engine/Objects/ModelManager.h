#pragma once
#include "ModelObject.h"
#include <unordered_map>


//class ModelManager
//{
//public:
//	static ModelManager* GetInstance() {
//		static ModelManager instance;
//		return &instance;
//	}
//public:
//	void Initialize(Fngine* fngine);
//	int LoadObj(const std::string& filename, const std::string& directoryPath = "resources");
//	void DrawModel(ModelID modelId);
//private:
//	// 図鑑的な存在
//	std::vector<std::unordered_map<ModelID, std::unique_ptr<ModelObject>>>models_;
//	// 出力するのはこっち
//	std::vector<std::unique_ptr<ModelObject>>drawModels_;
//	uint32_t modelCount_;
//	Fngine* pFngine_;
//};

