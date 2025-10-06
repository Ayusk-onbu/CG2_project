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

	worldFence_ = std::make_unique<WorldTransform>();
	worldFence_->Initialize();
	worldFence_->set_.Rotation({ 0.0f,Deg2Rad(180),0.0f});
	worldFence_->set_.Scale({ 5.0f,5.0f,5.0f });
	fenceObj_ = std::make_unique<ModelObject>();
	fenceObj_->Initialize(p_fngine_->GetD3D12System(), "fence.obj","resources/fence");
}

void GameScene::Update(){
	useCamera_->UpDate();
	Vector3 pos = worldTransform_[0]->get_.Translation();
	/*ImGui::Begin("sprite");
	ImGuiManager::CreateImGui("translat",pos, -20.0f, 20.0f);
	ImGui::ColorEdit4("color", &color_.x);
	ImGui::End();*/
	worldTransform_[0]->set_.Translation(pos);
	sprite_[0]->SetColor(color_);

	Vector3 fencePos = worldFence_->get_.Translation();
	/*ImGui::Begin("fence");
	ImGuiManager::CreateImGui("fenceTranslat", fencePos, -20.0f, 20.0f);
	ImGui::End();*/
	worldFence_->set_.Translation(fencePos);
	fenceObj_->SetColor(color_);

	ImGui::Begin("BlendMode");
	if (ImGui::Button("Alpha")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::AlphaBlend);
	}
	if (ImGui::Button("Add")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Additive);
	}
	if (ImGui::Button("Sub")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Subtractive);
	}
	if (ImGui::Button("Mul")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::Multiplicative);
	}
	if (ImGui::Button("Screen")) {
		p_fngine_->GetPSO().SetBlendState(BLENDMODE::ScreenBlend);
	}
	ImGui::End();
}

void GameScene::Draw() {

	
	for (int i = 0;i < 1;++i) {
		worldTransform_[i]->LocalToWorld();
		sprite_[i]->SetWVPData(useCamera_->DrawCamera(worldTransform_[i]->mat_), worldTransform_[i]->mat_, Matrix4x4::Make::Identity());
		sprite_[i]->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *texture_);
	}
	worldTransform_[1]->LocalToWorld();
	sprite_[1]->SetWVPData(useCamera_->DrawCamera(worldTransform_[1]->mat_), worldTransform_[1]->mat_, Matrix4x4::Make::Identity());
	sprite_[1]->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(), *texture_);

	worldFence_->LocalToWorld();
	fenceObj_->SetWVPData(useCamera_->DrawCamera(worldFence_->mat_), worldFence_->mat_, Matrix4x4::Make::Identity());
	fenceObj_->Draw(p_fngine_->GetCommand(), p_fngine_->GetPSO(), p_fngine_->GetLight(),*texture_);
}