#include "Player3D.h"
#include "CameraSystem.h"

void Player3D::Initialize(Fngine* fngine)
{
	obj_ = std::make_unique<ModelObject>();
	obj_->Initialize(fngine->GetD3D12System(),"axis.obj");
	obj_->SetFngine(fngine);//sdugasigdi
	obj_->textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
}

void Player3D::Update()
{

	Vector3 pos = obj_->worldTransform_.get_.Translation();
	move_ = { 0.0f,0.0f,0.0f };

	Move(pos);

	move_ = move_.z * CameraSystem::GetInstance()->GetActiveCamera()->zAxis_ + move_.x * CameraSystem::GetInstance()->GetActiveCamera()->xAxis_;
	move_ = Normalize(move_);

	//CameraSystem::GetInstance()->GetActiveCamera()->targetPos_ = obj_->worldTransform_.transform_.translation_;
	pos += move_ * 5.0f;
	obj_->worldTransform_.set_.Translation(pos);

	ImGuiManager::GetInstance()->DrawDrag("PlayerPos", obj_->worldTransform_.get_.Translation());
}

void Player3D::Draw()
{
	obj_->LocalToWorld();
	obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
	obj_->Draw();
}

void Player3D::Move(Vector3& pos) {
	if (InputManager::GetKey().PressKey(DIK_W))
	{
		move_.z = 1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_S))
	{
		move_.z = -1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_A))
	{
		move_.x = -1.0f;
	}
	if (InputManager::GetKey().PressKey(DIK_D))
	{
		move_.x = 1.0f;
	}
}
