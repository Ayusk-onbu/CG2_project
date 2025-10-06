#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"

// ================================
// Override Functions
// ================================

void TestScene::Initialize() {



}

void TestScene::Update() {

	ImGuiManager::GetInstance()->DrawSlider("Test1",test_,1.0f,5.0f);
	ImGuiManager::GetInstance()->DrawSlider("Test1z", test2_, 1.0f, 5.0f);
	ImGuiManager::GetInstance()->DrawDrag("Test2", test2_);
	ImGuiManager::GetInstance()->DrawSlider("TestM", testM_,0.0f,1.0f);

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {

}