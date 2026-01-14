#include "SceneDirector.h"
#include "CameraSystem.h"
#include "ModelManager.h"

SceneDirector::~SceneDirector() {
	delete currentScene_;
}

void SceneDirector::Initialize(Scene& firstScene) {
	LoadModelData();
	LoadTexture();
	LoadMusic();

	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);

	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->MakeCamera("GameCamera", CameraType::Game);
	CameraSystem::GetInstance()->SetActiveCamera("GameCamera");
}

void SceneDirector::Update() {
	CameraSystem::GetInstance()->Update();
	p_fngine_->GetCameraForGPU().Update(CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation());
	//while()
	if (isGameRunning_) {
		currentScene_->Update();
	}
	p_fngine_->GetPointLight().Update();
	p_fngine_->GetSpotLight().Update();
}

void SceneDirector::Draw() {
	currentScene_->Draw();
}

void SceneDirector::ImGui() {
	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : pos", p_fngine_->GetLight().directionalLightData_->direction, -1.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : color", p_fngine_->GetLight().directionalLightData_->color, 0.0f, 1.0f);
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
	name = ModelManager::GetInstance()->LoadObj("bullet.obj", "resources", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("debugBlock.obj", "resources", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("plane.gltf", "resources");
}

void SceneDirector::LoadTexture() {
	std::string name;
	name = TextureManager::GetInstance()->LoadTexture("GridLine.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("ulthimaSky.png","resources");
	name = TextureManager::GetInstance()->LoadTexture("Confuse.png","resources/Star");
	name = TextureManager::GetInstance()->LoadTexture("bullet.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("circle.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("Legends_Ground.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("GameClear.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("GameOver.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("Purpose.png", "resources/Title");
	name = TextureManager::GetInstance()->LoadTexture("UI.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("titleBack.png", "resources/Title");
}

void SceneDirector::LoadMusic() {
	std::string name;
	name = Music::GetInstance()->LoadSE("loop101204.wav", "resources");
}