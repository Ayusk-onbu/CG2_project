#include "SceneDirector.h"
#include "CameraSystem.h"
#include "ModelManager.h"
#include "UIAnimation.h"
#include "Chronos.h"
#include "DrawManager.h"
#include "SDFManager.h"
#include "AnimationManager.h"
#include "../Engine/Objects/Primitive/Box/PrimitiveBox.h"
#include "../Engine/Objects/Primitive/Ring/Ring.h"
#include "../Engine/Objects/Primitive/MagicPlane.h"
#include "../UsefulTool/EditorManager/Hair/HairEditor.h"
#include "../UsefulTool/EditorManager/Map/SceneEditor.h"
#include "../UsefulTool/EditorManager/Hermite/HermiteEditor.h"
#include "../UsefulTool/Component/ComponentFactory.h"
#include <pix3.h>

import MotionManager;

SceneDirector::~SceneDirector() {
	delete currentScene_;
	PrimitiveBox::GetInstance()->ReleaseInstance();
	PrimitiveRing::GetInstance()->ReleaseInstance();
}

void SceneDirector::Initialize(Scene& firstScene) {
	// Loadは何か他のクラスにまとめるべきなのではないのであろうか
	LoadTexture();
	LoadModelData();
	LoadMusic();
	UIHAnimationManager::GetInstance()->Load();
	MotionManager::GetInstance()->LoadMotions("resources/Data/Motion/Hermite/");

	SDFManager::GetInstance()->Initialize(p_fngine_);
	SDFManager::GetInstance()->LoadAndBake(p_fngine_->GetCommand().GetList().GetList().Get(), "Naira_ExportTest", 256);

	// 最初のシーンの初期化処理
	currentScene_ = &firstScene;
	currentScene_->FngineSetUp(*p_fngine_);
	currentScene_->Initialize();
	currentScene_->GetSceneDirector(this);

	// 使うカメラの作成
	CameraSystem::GetInstance()->MakeCamera("NormalCamera", CameraType::Normal);
	CameraSystem::GetInstance()->MakeCamera("DebugCamera", CameraType::Debug);
	CameraSystem::GetInstance()->MakeCamera("GameCamera", CameraType::Game);
	CameraSystem::GetInstance()->SetActiveCamera("GameCamera");

	ComponentFactory::GetInstance()->Initialize();

	DrawManager::GetInstance()->Initialize(p_fngine_);
	PrimitiveBox::GetInstance()->Initialize(p_fngine_, 1500);
	PrimitiveRing::GetInstance()->Initialize(p_fngine_, 1000);

	auto editorMgr = EditorManager::GetInstance();
	auto* hairEditor = editorMgr->CreateEditor<HairGuideEditor>(p_fngine_->hair_.get());
	auto* hermiteEditor = editorMgr->CreateEditor<HermiteEditor>();
	editorMgr->SetActiveEditor(hermiteEditor);
}

void SceneDirector::Update() {
	// 更新処理

	// [ カメラ ]
	CameraSystem::GetInstance()->Update();
	// [ カメラの情報をGPUへ ]
	p_fngine_->GetCameraForGPU().Update(CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation());

	TimeKeeper::GetInstance()->Update(1.0f / 60.0f);

	bool pauseNow = PauseSystem::GetInstance()->Update(currentScene_->CanPause());

	// [ *** ポーズ中じゃない *** ]
	if (pauseNow == false) {
		// [ ゲーム ]
		if (isGameRunning_) {
			// [ シーン ]
			currentScene_->Update();
		}
	}
	// [ *** ポーズ中 *** ]
	else if (pauseNow == true) {
		currentScene_->PauseUpdate();
	}

	// [ ライト ] 
	p_fngine_->GetPointLight().Update();
	p_fngine_->GetSpotLight().Update();
	p_fngine_->GetAreaLight().Update();

}

void SceneDirector::Draw() {
	// 描画処理
	bool pauseNow = PauseSystem::GetInstance()->GetIsPause();
	
	// [ ポーズ中じゃない ]
	if (pauseNow == false) {
		// [ ゲーム ]
		if (isGameRunning_) {
			// [ シーン ]
			currentScene_->Draw();

			PrimitiveBox::GetInstance()->DrawInstanced();
			PrimitiveRing::GetInstance()->DrawInstanced();
		}
	}
	// [ ポーズ中 ]
	else if (pauseNow == true) {
		currentScene_->PauseDraw();
	}

	DrawManager::GetInstance()->Draw();
}

void SceneDirector::ImGui() {
	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : pos", p_fngine_->GetLight().directionalLightData_->direction, -1.0f, 1.0f);
	ImGuiManager::GetInstance()->DrawSlider("DirectionalLight : color", p_fngine_->GetLight().directionalLightData_->color, 0.0f, 1.0f);
	//ImGuiManager::GetInstance()->DrawSlider("Dissolve : threshold", p_fngine_->dissolveForGPU_->GetMappedData()->threshold, 0.0f, 1.0f);
	//ImGuiManager::GetInstance()->DrawSlider("Dissolve : edgeMin", p_fngine_->dissolveForGPU_->GetMappedData()->edgeMin, 0.0f, 1.0f);
	//ImGuiManager::GetInstance()->DrawSlider("Dissolve : edgeMax", p_fngine_->dissolveForGPU_->GetMappedData()->edgeMax, 0.0f, 1.0f);
	PSOManager::GetInstance()->ImGui();
#ifdef USE_IMGUI
	if (ImGui::TreeNode("SceneDirector")) {

		if (ImGui::Button("TestScene")) {
			RequestChangeScene(new TestScene());
		}
		ImGui::SameLine();
		if (ImGui::Button("TitleScene")) {
			RequestChangeScene(new TitleScene());
		}
		ImGui::SameLine();
		if (ImGui::Button("GameScene")) {
			RequestChangeScene(new GameScene());
		}
		ImGui::SameLine();
		if (ImGui::Button("GameOverScene")) {
			RequestChangeScene(new GameOverScene());
		}
		ImGui::SameLine();
		if (ImGui::Button("GameClearScene")) {
			RequestChangeScene(new ClearScene());
		}

		ImGui::TreePop();
	}
	EditorManager::GetInstance()->UpdateEditors();
#endif//USE_IMGUI
}

void SceneDirector::RequestChangeScene(Scene* newScene) {
	if (currentScene_) {
		// 今使っているsceneを削除
		delete currentScene_;
	}

	// そんなことは起きないと思うけど一応
	// ポーズが切れるようにする
	PauseSystem::GetInstance()->SetPause(false);

	// 新しいsceneを取得
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
	name = ModelManager::GetInstance()->LoadObj("Cylinder.obj", "resources");
	name = ModelManager::GetInstance()->LoadObj("BoneOctahedron.obj", "resources");
	name = ModelManager::GetInstance()->LoadObj("Column.obj", "resources");
	name = ModelManager::GetInstance()->LoadObj("Sphere.obj", "resources");
	name = ModelManager::GetInstance()->LoadObj("ulthimaSky.obj", "resources", LoadFileType::OBJ);
	name = ModelManager::GetInstance()->LoadObj("Map_City.obj", "resources/Data/Map");
	name = ModelManager::GetInstance()->LoadObj("Naira_ExportTest.gltf", "resources/Model/Character/Test");

	ModelManager::GetInstance()->AddObject("Cube", ModelManager::GetInstance()->LoadModelData("AnimatedCube").vertices, ModelManager::GetInstance()->LoadModelData("AnimatedCube").indices);
	ModelManager::GetInstance()->AddObject("Plane", ModelManager::GetInstance()->LoadModelData("plane").vertices, ModelManager::GetInstance()->LoadModelData("plane").indices);
	ModelManager::GetInstance()->AddObject("Ring", MakeObjectVertices(RingData{ 32, 1.0f, 0.5f }), MakeObjectIndices(RingData{ 32, 1.0f, 0.5f }));
	ModelManager::GetInstance()->AddObject("Sphere", ModelManager::GetInstance()->LoadModelData("Sphere").vertices, ModelManager::GetInstance()->LoadModelData("Sphere").indices);
	ModelManager::GetInstance()->AddObject("Cylinder", MakeObjectVertices(CylinderData{ 32, 1.0f, 0.5f,1.0f }), MakeObjectIndices(CylinderData{ 32, 1.0f, 0.5f, 1.0f }));
	//ModelManager::GetInstance()->AddObject("Grass", MakeObjectVertices(CylinderData{ 3, 0.05f * 0.1f, 0.5f * 0.1f,0.5f }), MakeObjectIndices(CylinderData{ 3, 0.05f * 0.1f, 0.5f * 0.1f, 0.5f }));
	ModelManager::GetInstance()->AddObject("Grass", ModelManager::GetInstance()->LoadModelData("AnimatedCube").vertices, ModelManager::GetInstance()->LoadModelData("AnimatedCube").indices);
	ModelManager::GetInstance()->AddObject("Column", ModelManager::GetInstance()->LoadModelData("Column").vertices, ModelManager::GetInstance()->LoadModelData("Column").indices);
	ModelManager::GetInstance()->AddObject("BoneOctahedron", ModelManager::GetInstance()->LoadModelData("BoneOctahedron").vertices, ModelManager::GetInstance()->LoadModelData("BoneOctahedron").indices);


	AnimationManager::GetInstance()->LoadAnimationFile("resources/Human", "walk.gltf");
}

void SceneDirector::LoadTexture() {
	TextureManager::GetInstance()->BeginLoad();

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
	name = TextureManager::GetInstance()->LoadTexture("TitleBack.png", "resources/Title");
	name = TextureManager::GetInstance()->LoadTexture("Title.png", "resources/Title");
	name = TextureManager::GetInstance()->LoadTexture("Tiles083_1K-PNG_Color.png", "Assets/Textures/Color");
	name = TextureManager::GetInstance()->LoadTexture("Tiles083_1K-PNG_NormalDX.png", "Assets/Textures/Normal");
	name = TextureManager::GetInstance()->LoadTexture("monsterBall.png", "resources");
	name = TextureManager::GetInstance()->LoadTexture("PlayGuide.png", "resources/Game");
	name = TextureManager::GetInstance()->LoadTexture("rostock_laage_airport_4k.dds", "resources");
	name = TextureManager::GetInstance()->LoadTexture("magic-circle.png", "resources/Texture/Effect");
	name = TextureManager::GetInstance()->LoadTexture("RevealTex_Circle.png", "resources/Texture/Effect");
	name = TextureManager::GetInstance()->LoadTexture("noise0.png", "resources/Texture/Effect");
	name = TextureManager::GetInstance()->LoadTexture("gradationLine.png", "resources/Texture/Effect");
	TextureManager::GetInstance()->EndLoad();

	//Created using <asset name> from ambientCG.com,
	//licensed under the Creative Commons CC0 1.0 Universal License.
}

void SceneDirector::LoadMusic() {
	std::string name;
	name = Music::GetInstance()->LoadSE("loop101204.wav", "resources");
}