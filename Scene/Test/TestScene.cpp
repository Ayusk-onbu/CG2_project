#include "TestScene.h"
#include "SceneDirector.h"
#include "ImGuiManager.h"

// ================================
// Override Functions
// ================================

void TestScene::Initialize() {


	rotation_ = Quaternion::MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	rotation2_ = Quaternion::MakeRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);
}

void TestScene::Update() {

	ImGuiManager::GetInstance()->DrawSlider("Test1",test_,1.0f,5.0f);
	ImGuiManager::GetInstance()->DrawSlider("Test1z", test2_, 1.0f, 5.0f);
	ImGuiManager::GetInstance()->DrawDrag("Test2", test2_);
	ImGuiManager::GetInstance()->DrawSlider("TestM", testM_,0.0f,1.0f);

	Quaternion interpolated = Quaternion::Slerp(rotation_, rotation2_, 0.0f);	
	re1_.x = interpolated.x;re1_.y = interpolated.y;re1_.z = interpolated.z;re1_.w = interpolated.w;
	interpolated = Quaternion::Slerp(rotation_, rotation2_, 0.3f);
	re2_.x = interpolated.x;re2_.y = interpolated.y;re2_.z = interpolated.z;re2_.w = interpolated.w;
	interpolated = Quaternion::Slerp(rotation_, rotation2_, 0.5f);
	re3_.x = interpolated.x;re3_.y = interpolated.y;re3_.z = interpolated.z;re3_.w = interpolated.w;
	interpolated = Quaternion::Slerp(rotation_, rotation2_, 0.7f);
	re4_.x = interpolated.x;re4_.y = interpolated.y;re4_.z = interpolated.z;re4_.w = interpolated.w;
	interpolated = Quaternion::Slerp(rotation_, rotation2_, 1.0f);
	re5_.x = interpolated.x;re5_.y = interpolated.y;re5_.z = interpolated.z;re5_.w = interpolated.w;

	ImGuiManager::GetInstance()->Text("Result");
	ImGuiManager::GetInstance()->DrawDrag("re1", re1_);
	ImGuiManager::GetInstance()->DrawDrag("re2", re2_);
	ImGuiManager::GetInstance()->DrawDrag("re3", re3_);
	ImGuiManager::GetInstance()->DrawDrag("re4", re4_);
	ImGuiManager::GetInstance()->DrawDrag("re5", re5_);

	if (hasRequestedNextScene_) {
		p_sceneDirector_->RequestChangeScene(new GameScene());
	}
}

void TestScene::Draw() {

}