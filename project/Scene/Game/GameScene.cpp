#include "GameScene.h"
#include "ImGuiManager.h"
#include "CameraSystem.h"
#include "SceneDirector.h"


GameScene::GameScene()
	: player_(std::make_unique<Player3D>()),
	  boss_(std::make_unique<BossEnemy>()),
	  collisionManager_(std::make_unique<CollisionManager>()),
	  ground_(std::make_unique<Ground>())
{

}

GameScene::~GameScene() {
	
}

void GameScene::Initialize() {
	player_->Initialize(p_fngine_);
	boss_->Initialize(p_fngine_,player_.get());
	ground_->Initialize(p_fngine_);
	toGameTimer_ = 0.0f;
	// Fade関係のUI
	fadeUp_ = std::make_unique<SpriteObject>(p_fngine_);
	fadeUp_->Initialize("GridLine");
	fadeUp_->worldTransform_.set_.Translation({ 640.0f,180.0f,0.0f });
	fadeUp_->worldTransform_.set_.Scale({ 1280.0f / 16.0f, 360.0f / 16.0f,0.0f });
	fadeUp_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	fadeDown_ = std::make_unique<SpriteObject>(p_fngine_);
	fadeDown_->Initialize("GridLine");
	fadeDown_->worldTransform_.set_.Translation({ 640.0f,180.0f + 360.0f,0.0f });
	fadeDown_->worldTransform_.set_.Scale({ 1280.0f / 16.0f, 360.0f / 16.0f,0.0f });
	fadeDown_->SetColor({ 0.0f,0.0f,0.0f,1.0f });

	purposeUI_ = std::make_unique<SpriteObject>(p_fngine_);
	purposeUI_->Initialize("Purpose");
	purposeUI_->worldTransform_.set_.Translation({ 640.0f,180.0f,0.0f });
	purposeUI_->worldTransform_.set_.Scale({ 1.0f, 1.0f,0.0f });
	purposeUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	playUI_ = std::make_unique<SpriteObject>(p_fngine_);
	playUI_->Initialize("UI");
	playUI_->worldTransform_.set_.Translation({ 1280.0f - 128.0f - 40.0f,64.0f + 50.0f,0.0f });
	playUI_->worldTransform_.set_.Scale({ 0.65f, 0.65f,0.0f });
	playUI_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

	// ポーズ関係のUI
	pause_ = std::make_unique<SpriteObject>(p_fngine_);
	pause_->Initialize("GridLine");
	pause_->worldTransform_.set_.Scale({ 1280.0f / 16.0f, 720.0f / 16.0f, 0.0f });
	pause_->worldTransform_.set_.Translation({ 640.0f, 360.0f, 0.0f });
	pause_->SetColor({ 0.2f, 0.2f, 0.2f, 0.5f });
}

void GameScene::Update(){
	float deltaTime = 1.0f / 60.0f;
	if (notGame_) {
		toGameTimer_ += deltaTime;
		FirstFade(toGameTimer_);
		if (boss_->StartCutscene(toGameTimer_)) {
			notGame_ = false;
		}
		ImGuiManager::GetInstance()->Text("Not Game");
	}
	else {
		player_->Update();

		//boss_->Update();

		ground_->Update();

		CollisionCheck();

		ToScene();
	}
}

void GameScene::Draw() {
	ground_->Draw();
	boss_->Draw();
	player_->Draw();

	playUI_->Draw();
	// Fade
	fadeUp_->Draw();
	fadeDown_->Draw();
	// UI
	purposeUI_->Draw();

}

void GameScene::CollisionCheck() {
	// 中身をclear
	collisionManager_->Begin();

	// ここからColliderを設定
	collisionManager_->SetColliders(&player_->GetAttackCollider());
	collisionManager_->SetColliders(&player_->GetPlayerBodyCollider());
	collisionManager_->SetColliders(&boss_->GetAttackCollider());
	collisionManager_->SetColliders(&boss_->GetBossBodyCollider());
	for (auto& bullet : boss_->GetBulletManager().bullets_) {
		collisionManager_->SetColliders(&bullet->GetCollider());
	}
	

	// Check!
	collisionManager_->CheckAllCollisions();
}

void GameScene::ToScene() {
	if (toSceneTimer_ < 3.0f) {
		// timeを0にして使う
		Vector3 upPos = fadeUp_->worldTransform_.get_.Translation();
		upPos.y = Easing_Float(-180.0f, 180.0f, toSceneTimer_, 3.0f, EASINGTYPE::InBounce);
		fadeUp_->worldTransform_.set_.Translation(upPos);

		Vector3 downPos = fadeDown_->worldTransform_.get_.Translation();
		downPos.y = Easing_Float((180.0f) + 720.0f, 180.0f + 360.0f, toSceneTimer_, 3.0f, EASINGTYPE::InBounce);
		fadeDown_->worldTransform_.set_.Translation(downPos);
	}
	if (boss_->IsDead()) {
		ToClearScene();
	}
	else if (player_->IsDead()) {
		ToGameOverScene();
	}
}

void GameScene::ToClearScene() {
	toSceneTimer_ += 1.0f / 60.0f;

	if (toSceneTimer_ > 3.0f) {
		p_sceneDirector_->RequestChangeScene(new ClearScene());
		return;
	}
}

void GameScene::ToGameOverScene() {
	toSceneTimer_ += 1.0f / 60.0f;

	if (toSceneTimer_ > 3.0f) {
		p_sceneDirector_->RequestChangeScene(new GameOverScene());
		return;
	}
}

void GameScene::FirstFade(float time) {
	if (time < 0.2f) {
		// 開ける
		Vector3 upPos = fadeUp_->worldTransform_.get_.Translation();
		upPos.y = Easing_Float(180.0f, 0.0f, time, 0.2f, EASINGTYPE::None);
		fadeUp_->worldTransform_.set_.Translation(upPos);

		Vector3 downPos = fadeDown_->worldTransform_.get_.Translation();
		downPos.y = Easing_Float(180.0f + 360.0f, 720.0f, time, 0.2f, EASINGTYPE::None);
		fadeDown_->worldTransform_.set_.Translation(downPos);
	}
	else if (time < 3.0f && time > 2.0f) {
		// timeを0にして使う
		float fadeOutTime = time - 2.0f;
		Vector3 upPos = fadeUp_->worldTransform_.get_.Translation();
		upPos.y = Easing_Float(0.0f, -180.0f, fadeOutTime, 1.0f, EASINGTYPE::InBounce);
		fadeUp_->worldTransform_.set_.Translation(upPos);

		Vector3 downPos = fadeDown_->worldTransform_.get_.Translation();
		downPos.y = Easing_Float(720.0f, (180.0f) + 720.0f, fadeOutTime, 1.0f, EASINGTYPE::InBounce);
		fadeDown_->worldTransform_.set_.Translation(downPos);
	}
	float alpha = 1.0f;
	alpha = Easing_Float(1.0f, 0.0f, toGameTimer_, 3.0f, EASINGTYPE::None);
	purposeUI_->SetColor({ 1.0f,1.0f,1.0f,alpha });
}

void GameScene::PauseUpdate() {

}

void GameScene::PauseDraw() {
	ground_->Draw();
	boss_->Draw();
	player_->Draw();

	pause_->Draw();
}