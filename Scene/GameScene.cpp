#include "GameScene.h"
#include "ImGuiManager.h"

GameScene::~GameScene() {
	
}

void GameScene::Initialize() {
	// Initialization code for the game scene
	mainCamera_ = std::make_unique<Camera>();
	mainCamera_->Initialize();
	fpsCamera_ = std::make_unique<Camera>();
	fpsCamera_->Initialize();

	useCamera_ = &(*mainCamera_);
	//SetCamera(*mainCamera_);

	sprite_.resize(2);
	for (int i = 0;i < sprite_.size();++i) {
		sprite_[i] = std::make_unique<SpriteObject>();
		sprite_[i]->Initialize(p_fngine_->GetD3D12System(), 10.0f, 10.0f);
		sprite_[i]->SetColor({ 1.0f,1.0f,1.0f,0.5f });
	}
	worldTransform_.resize(2);
	for (int i = 0;i < worldTransform_.size();++i) {
		worldTransform_[i] = std::make_unique<WorldTransform>();
		worldTransform_[i]->Initialize();
		worldTransform_[i]->set_.Translation({ 1.0f + i,1.0f,1.0f });
		worldTransform_[i]->set_.Scale({ 1.0f,1.0f,1.0f });
		worldTransform_[i]->set_.Rotation({ 0.0f,Deg2Rad(180),0.0f });
	}
	texture_ = std::make_unique<Texture>();
	texture_->Initialize(p_fngine_->GetD3D12System(), p_fngine_->GetSRV(),"resources/uvChecker.png",1);
}

void GameScene::Update(){
	useCamera_->UpDate();
	Vector3 pos = worldTransform_[0]->get_.Translation();
	ImGui::Begin("sprite");
	ImGuiManager::CreateImGui("translat",pos, -20.0f, 20.0f);
	ImGui::End();
	worldTransform_[0]->set_.Translation(pos);

	ImGui::Begin("BlendMode");

	ImGui::End();
}

void GameScene::Draw() {

	
	for (int i = 0;i < 2;++i) {
		worldTransform_[i]->LocalToWorld();
		sprite_[i]->SetWVPData(useCamera_->DrawCamera(worldTransform_[i]->mat_), worldTransform_[i]->mat_, Matrix4x4::Make::Identity());
		sprite_[i]->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *texture_);
	}
}