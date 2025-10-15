#pragma once
#include "Camera.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

class CameraSystem final
{
public:
	CameraSystem() = default;
	~CameraSystem() = default;
	static CameraSystem* GetInstance() {
		static CameraSystem instance;
		return &instance;
	}
public:
	void MakeCamera(std::string name,CameraType cameraType);
	void Update();
	void SetActiveCamera(std::string name);
	Camera* GetActiveCamera();
private:
	// 複数のカメラを名前と一緒に管理する
	std::unordered_map<std::string, std::vector<std::shared_ptr<Camera>>> cameras_;
	std::shared_ptr<Camera> activeCamera_;
};