#include "SceneDirector.h"
#include "CameraSystem.h"
#include "ModelManager.h"

SceneDirector::~SceneDirector() {
	delete currentScene_;
}

void SceneDirector::Initialize(Scene& firstScene) {
	LoadModelData();

	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);

	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->MakeCamera("GameCamera", CameraType::Game);
	CameraSystem::GetInstance()->SetActiveCamera("GameCamera");
}

void SceneDirector::Run() {
	CameraSystem::GetInstance()->Update();
	p_fngine_->GetCameraForGPU().Update(CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation());
	//while()
	if (isGameRunning_) {
		currentScene_->Update();
	}
	currentScene_->Draw();

	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : pos", p_fngine_->GetLight().directionalLightData_->direction,-1.0f,1.0f);
	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : color", p_fngine_->GetLight().directionalLightData_->color, 0.0f, 1.0f);
	p_fngine_->GetPointLight().Update();
	p_fngine_->GetSpotLight().Update();

	PSOManager::GetInstance()->ImGui();
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

void SceneDirector::LoadModelData() {
	std::string name;
	name = ModelManager::GetInstance()->LoadObj("cube.obj","resources",LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("axis.obj", "resources", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("AnimatedCube.gltf", "resources/AnimatedCube");
	name = ModelManager::GetInstance()->LoadObj("walk.gltf", "resources/Human");
	name = ModelManager::GetInstance()->LoadObj("sneakWalk.gltf", "resources/Human");
	name = ModelManager::GetInstance()->LoadObj("simpleSkin.gltf", "resources/simpleSkin");
	name = ModelManager::GetInstance()->LoadObj("ground.obj", "resources", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("enemy.obj", "resources/Game", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("player.obj", "resources/Game", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("Confuse.obj", "resources/Star", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("stone.obj", "resources/Game", LoadFileType::OBJ);
}