#include "SceneDirector.h"

SceneDirector::~SceneDirector() {
	delete currentScene_;
}

void SceneDirector::Initialize(Scene& firstScene) {
	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);
}

void SceneDirector::Run() {
	//while()
	currentScene_->Update();
	currentScene_->Draw();
}

void SceneDirector::RequestChangeScene(Scene* newScene) {
	if (currentScene_) {
		delete currentScene_;
	}
	currentScene_ = newScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);
}