#include "SceneDirector.h"

void SceneDirector::Run() {
	//while()
	currentScene_->Update();
	currentScene_->Draw();
}

void SceneDirector::ChangeScene(Scene& nextScene) {
	currentScene_ = &nextScene;
}