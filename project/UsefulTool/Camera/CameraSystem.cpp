#include "CameraSystem.h"

void CameraSystem::Update() {
	/*for (auto& pair : cameras_) {
		for (auto& camera : pair.second) {
			camera->Update();
		}
	}*/
	activeCamera_->Update();
}

void CameraSystem::SetActiveCamera(std::string name) {
	if (cameras_.find(name) != cameras_.end()) {
		activeCamera_ = cameras_[name].back();
	}
}

void CameraSystem::MakeCamera(std::string name,CameraType cameraType) {
	cameras_[name].emplace_back(std::make_unique<Camera>());
	cameras_[name].back()->Initialize();
	cameras_[name].back()->AddControllers(cameraType);
}

Camera* CameraSystem::GetActiveCamera() {
	return activeCamera_.get();
}