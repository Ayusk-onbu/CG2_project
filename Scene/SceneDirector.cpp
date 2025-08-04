#include "SceneDirector.h"

void SceneDirector::Initialize(Scene& firstScene) {
	currentScene_ = &firstScene;
}

void SceneDirector::Run() {
	//while()
	currentScene_->Update();
	currentScene_->Draw();
}

void SceneDirector::ChangeScene(Scene& nextScene) {
	currentScene_ = &nextScene;
	currentScene_->Initialize();
}