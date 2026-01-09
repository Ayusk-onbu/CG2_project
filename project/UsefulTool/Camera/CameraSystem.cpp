#include "CameraSystem.h"
#include "ImGuiManager.h"

void CameraSystem::Update() {
	activeCamera_->Update();
#ifdef USE_IMGUI
	ImGui::Begin("Cameras");
	for (auto& camera : cameras_) {
		if (ImGui::Button(camera.first.c_str())) {
			SetActiveCamera(camera.first);
		}
	}
	ImGui::End();
#endif// USE_IMGUI
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

void CameraSystem::ImGui() {
	/*ここからImGuiの処理を施す*/
}