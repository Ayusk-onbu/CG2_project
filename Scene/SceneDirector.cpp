#include "SceneDirector.h"

void SceneDirector::Run() {
	//while()
	currentScene_->Update();
	currentScene_->Draw();
}

void SceneDirector::ChangeScene(std::unique_ptr<Scene> nextScene) {
	currentScene_ = move(nextScene);
}