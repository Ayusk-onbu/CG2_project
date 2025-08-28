#include "SceneDirector.h"

SceneDirector::~SceneDirector() {
	delete currentScene_;
}

void SceneDirector::Initialize(Scene& firstScene) {
	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
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