#include "GameScene.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
	delete currentPhase_;
}

void GameScene::Initialize() {
	// Initialization code for the game scene
	mainCamera_ = std::make_unique<Camera>();
	mainCamera_->Initialize();
	fpsCamera_ = std::make_unique<Camera>();
	fpsCamera_->Initialize();

	useCamera_ = &(*mainCamera_);
	//SetCamera(*mainCamera_);

	title_ = std::make_unique<Title>();
	titleModel_ = std::make_unique<ModelObject>();
	titleTexture_ = std::make_unique<Texture>();
	ground_ = std::make_unique<Ground>();
	groundModel_ = std::make_unique<ModelObject>();
	groundTexture_ = std::make_unique<Texture>();
	wall_ = std::make_unique<Wall>();
	wallModel_ = std::make_unique<ModelObject>();
	wallTexture_ = std::make_unique<Texture>();
	player_ = std::make_unique<Player>();
	playerModel_ = std::make_unique<ModelObject>();
	playerTexture_ = std::make_unique<Texture>();
	enemy_ = std::make_unique<Enemy>();
	enemyModel_ = std::make_unique<ModelObject>();
	enemyTexture_ = std::make_unique<Texture>();
	for (int i = 0;i < (int)potCount_;i++) {
		std::unique_ptr<Pot> newPot = std::make_unique<Pot>();	
		pots_.push_back(std::move(newPot));
	}
	potModel_ = std::make_unique<ModelObject>();
	potTexture_ = std::make_unique<Texture>();
	failedClear_ = std::make_unique<FailedClear>();
	failedModel_ = std::make_unique<ModelObject>();
	failedTexture_ = std::make_unique<Texture>();
	clearModel_ = std::make_unique<ModelObject>();
	clearTexture_ = std::make_unique<Texture>();

	t_ = std::make_unique<T>();
	tModel_ = std::make_unique<ModelObject>();
	ls_ = std::make_unique<LS>();
	lsModel_ = std::make_unique<ModelObject>();
	nazo_ = std::make_unique<Nazo>();
	nazoModel_ = std::make_unique<ModelObject>();

	titleModel_->Initialize(p_fngine_->GetD3D12System(), "Title.obj");
	title_->Initialize(&*titleModel_, useCamera_);
	groundModel_->Initialize(p_fngine_->GetD3D12System(), "hallway.obj","resources/hallway");
	groundTexture_->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), "resources/hallway/hallway.png", 1);
	ground_->Initialize(&*groundModel_, useCamera_);
	playerModel_->Initialize(p_fngine_->GetD3D12System(), "F-14.obj");
	playerTexture_->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), playerModel_->GetFilePath(), 2);
	player_->Initialize(&*playerModel_, useCamera_);

	enemyModel_->Initialize(p_fngine_->GetD3D12System(), "bullet.obj");
	enemyTexture_->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), enemyModel_->GetFilePath(), 3);
	enemy_->Initialize(&*enemyModel_, useCamera_);

	potModel_->Initialize(p_fngine_->GetD3D12System(), "pot.obj", "resources/pot");
	wallModel_->Initialize(p_fngine_->GetD3D12System(), "Wall.obj");
	wall_->Initialize(&*wallModel_, useCamera_);
	//potTexture_->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(), potModel_->GetFilePath(), 3);
	for (int i = 0;i < pots_.size();i++) {
		pots_[i]->Initialize(&*potModel_, useCamera_,*p_fngine_);
		pots_[i]->SetPosition({ (float)(i * 5 + 5),1.2f,4.0f });
	}
	pots_[0]->SetColor({1.0f,0.0f,0.0f,1.0f});
	pots_[1]->SetColor({ 0.0f,1.0f,0.0f,1.0f });
	pots_[2]->SetColor({ 0.0f,0.0f,1.0f,1.0f });

	failedModel_->Initialize(p_fngine_->GetD3D12System(), "Failed.obj");
	clearModel_->Initialize(p_fngine_->GetD3D12System(), "Clear.obj");
	failedClear_->Initialize(&*clearModel_, &*failedModel_, useCamera_);

	tModel_->Initialize(p_fngine_->GetD3D12System(), "T.obj");
	t_->Initialize(&*tModel_, useCamera_);
	lsModel_->Initialize(p_fngine_->GetD3D12System(), "LeftShift.obj");
	ls_->Initialize(&*lsModel_, useCamera_);
	nazoModel_->Initialize(p_fngine_->GetD3D12System(), "Nazo.obj");
	nazo_->Initialize(&*nazoModel_, useCamera_);

	currentPhase_ = new TitlePhase(&*this);
	currentPhase_->Initialize();
}

void GameScene::Update(){
	useCamera_->UpDate();
	currentPhase_->Update();
	currentPhase_->IsEnd();

	if (isGameClear_) {
		failedClear_->SetIsClear(true);
		isViewCF_ = true;
	}
	if (isFailed_) {
		failedClear_->SetIsClear(false);
		isViewCF_ = true;
	}
}

void GameScene::Draw() {
	title_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	ground_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	wall_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	nazo_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	player_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *playerTexture_);
	for (int i = 0;i < pots_.size();i++) {
		pots_[i]->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(),*groundTexture_);
	}
	if(isViewCF_)
	failedClear_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	if (isEnemyView_) {
		enemy_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *enemyTexture_);
		t_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
		ls_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *groundTexture_);
	}
	
}

void GameScene::ChangePhase(Phase* newPhase) {
	if (currentPhase_ != nullptr) {
		delete currentPhase_;
	}
	currentPhase_ = newPhase;
	currentPhase_->Initialize();
}

void GameScene::SetCamera(CameraBase& camera) {
	useCamera_ = &camera;
}