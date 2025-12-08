#pragma once
#include "ModelObject.h"
#include <unordered_map>


class ModelManager
{
public:
	static ModelManager* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<ModelManager>();
		}
		return instance_.get();
	}
	void ReleaseInstance() { instance_.reset(); }
public:
	void Initialize(Fngine* fngine);
	ModelData& LoadModelData(const std::string& ID);
	std::string LoadObj(const std::string& filename, const std::string& directoryPath = "resources");
private:
	static std::unique_ptr<ModelManager>instance_;
	// 図鑑的な存在
	// [ 最初ModelDataだけでいいかと思ったが、表示したいかもだし別にいいかという判断。ただ、オブジェクトプールしたいから将来的に変更の可能性 ]
	std::unordered_map<std::string, std::unique_ptr<ModelObject>>models_;
	uint32_t modelCount_;
	Fngine* pFngine_;
};

