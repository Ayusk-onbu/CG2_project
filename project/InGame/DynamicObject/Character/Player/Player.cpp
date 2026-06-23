#include "Player.h"
#include "../Engine/Objects/Primitive/Box/PrimitiveBox.h"
#include "../Engine/Objects/Primitive/Ring/Ring.h"
#include "../Engine/Objects/Primitive/MagicPlane.h"

Player::Player() {
	controller_ = std::make_unique<PlayerController>();
}

void Player::Initialize(Fngine* fngine) {
	Character::Initialize(fngine, "Naira_ExportTest", "ulthimaSky");

	status_.SetBaseSpeed(5.0f);

	skeleton_ = std::make_unique<Skeleton>();
	skeleton_->CreateSkeleton(obj_->GetNode());

	obj_->skinCluster_.Create(fngine, *skeleton_, obj_->GetModelData());


	MeshCollider* myCollider = CreateCollider<MeshCollider>();
	myCollider->SetMyType(COL_Player);
	myCollider->SetYourType(COL_Static_Map);

	auto aabb = bvh_->GetRoot()->bounds;
	float lengthX = aabb.GetSize().x / 1.5f;
	myCollider->SetVertices({
		{aabb.min.x + lengthX, aabb.min.y, aabb.min.z}, // 左下
		{aabb.max.x - lengthX, aabb.min.y, aabb.min.z}, // 右下
		{aabb.max.x - lengthX, aabb.max.y, aabb.min.z}, // 右上
		{aabb.min.x + lengthX, aabb.max.y, aabb.min.z}, // 左上

		{aabb.min.x + lengthX, aabb.min.y, aabb.max.z}, // 左下
		{aabb.max.x - lengthX, aabb.min.y, aabb.max.z}, // 右下
		{aabb.max.x - lengthX, aabb.max.y, aabb.max.z}, // 右上
		{aabb.min.x + lengthX, aabb.max.y, aabb.max.z}, // 左上
	});

	myCollider->onCollisionCallBack = [this](Collider* other, const Vector3& pushOut) {
		OnCollisionGround(other, pushOut);
		if (other->GetMyType() == COL_Static_Map) {
			MakeHitEffect();
		}
	};
}

void Player::Update(float deltaTime) {
	Character::Update(deltaTime);

	skeleton_->Update();
	obj_->skinCluster_.Update(*skeleton_);

	collider_->SetWorldPosition(obj_->worldTransform_.get_.Translation());
	MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(collider_.get());
	meshCollider->SetWorldMatrix(obj_->worldTransform_.mat_);

	for (auto& effect : hitEffect_) {
		// エフェクトの更新
		effect.currentTime += deltaTime;
		effect.color.w = 1.0f - (effect.currentTime / effect.lifeTime); // 徐々に透明にする

		/*PrimitiveBox::GetInstance()->AddInstance({
			effect.transform,
			effect.color
		});*/
	}
	hitEffect_.erase(std::remove_if(hitEffect_.begin(), hitEffect_.end(),
		[](const HitEffectInfo& effect) { return effect.currentTime >= effect.lifeTime; }),
		hitEffect_.end());

	//WorldTransform testTransform;
	//testTransform.Initialize();
	//testTransform.set_.Scale({10.0f,10.0f,10.0f});
	//testTransform.set_.Rotation({ 270.0f,0.0f, 0.0f});
	////testTransform.set_.Translation({ obj_->worldTransform_.GetWorldPos().x,obj_->worldTransform_.GetWorldPos().y + 1.2f,obj_->worldTransform_.GetWorldPos().z });
	//testTransform.set_.Translation({ 0.0f,0.01f,0.0f });
	//testTransform.LocalToWorld();

	//WorldTransform testUVTransform;
	//testUVTransform.Initialize();
	//float scale = 1.0f;
	//testUVTransform.set_.Scale({ scale,scale,scale });
	//testUVTransform.set_.Rotation({ 0.0f,0.0f, 0.0f });
	//testUVTransform.set_.Translation({ 0.0f,0.0f,0.0f });
	//testUVTransform.LocalToWorld();

	//MagicCircle::GetInstance()->AddInstance({
	//	testTransform,
	//	testUVTransform,
	//	{1.0f,0.0f,0.0f,1.0f}
	//});

	//MagicCircle::GetInstance()->Update();

	/*PrimitiveRing::GetInstance()->AddInstance({
		testTransform,
		testUVTransform,
		{1.0f,0.0f,0.0f,1.0f}
	});*/

	// この処理はここに書きたくない
	/*CameraSystem::GetInstance()->GetActiveCamera()->SetTargetPos(
		{ obj_->worldTransform_.get_.Translation().x,obj_->worldTransform_.get_.Translation().y + 1.0f ,obj_->worldTransform_.get_.Translation().z });*/

	//auto ImGui = ImGuiManager::GetInstance();
	//auto Camera = CameraSystem::GetInstance()->GetActiveCamera();
	//Matrix4x4 viewMat = Camera->GetViewMatrix();
	//Matrix4x4 projMat = Camera->GetProjectionMatrix();
	//ImGui->DrawGizmo(
	//	viewMat,
	//	projMat,
	//	obj_->worldTransform_.mat_,
	//	ImGuizmo::TRANSLATE, // 移動モード（ROTATE や SCALE に変更可能）
	//	ImGuizmo::LOCAL      // ローカル座標系
	//);
	//ImGui->DrawGizmo(
	//	viewMat,
	//	projMat,
	//	obj_->worldTransform_.mat_,
	//	ImGuizmo::ROTATE, // 移動モード（ROTATE や SCALE に変更可能）
	//	ImGuizmo::LOCAL      // ローカル座標系
	//);
	//ImGui->DrawGizmo(
	//	viewMat,
	//	projMat,
	//	obj_->worldTransform_.mat_,
	//	ImGuizmo::SCALE, // 移動モード（ROTATE や SCALE に変更可能）
	//	ImGuizmo::LOCAL      // ローカル座標系
	//);
	obj_->LocalToWorld();
}

void Player::Draw() {
	obj_->LocalToWorld();
	Character::Draw();
}

void Player::MakeHitEffect() {
	hitEffectCoolTimer_ -= 1.0f / 60.0f;
	if (hitEffectCoolTimer_ > 0.0f)return;
	auto rand = RandomUtils::GetInstance();
	for (int i = 0; i < 5; ++i) {
		HitEffectInfo info;
		info.transform.Initialize();
		info.transform.set_.Scale({ 0.05f,0.3f + 0.5f * rand->GetHighRandom().GetFloat(0.0f,1.0f),0.05f });
		info.transform.set_.Rotation({ ((float)rand->GetHighRandom().GetInt(0,360)),180.0f, ((float)rand->GetHighRandom().GetInt(0,360)) });
		info.transform.set_.Translation({ obj_->worldTransform_.GetWorldPos().x,obj_->worldTransform_.GetWorldPos().y + 0.2f,obj_->worldTransform_.GetWorldPos().z });
		info.transform.LocalToWorld();
		info.lifeTime = 1.0f;
		info.currentTime = 0.0f;
		info.color = Vector4(1.0f, 0.5f, 0.5f, 1.0f);
		hitEffect_.push_back(info);
	}
	hitEffectCoolTimer_ = 1.2f;
}