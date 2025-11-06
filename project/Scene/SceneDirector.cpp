#include "SceneDirector.h"
#include "CameraSystem.h"

SceneDirector::~SceneDirector() {
	delete currentScene_;
}

void SceneDirector::Initialize(Scene& firstScene) {
	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);

	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->SetActiveCamera("DebugCamera");
}

void SceneDirector::Run() {
	CameraSystem::GetInstance()->Update();
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