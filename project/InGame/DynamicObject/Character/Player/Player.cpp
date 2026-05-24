#include "Player.h"
#include "../Engine/Objects/Primitive/Box/PrimitiveBox.h"

Player::Player() {
	controller_ = std::make_unique<PlayerController>();
}

void Player::Initialize(Fngine* fngine) {
	Character::Initialize(fngine, "Naira_ExportTest", "ulthimaSky");

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
		effect.color.w -= 0.01f; // 徐々に透明にする

		PrimitiveBox::GetInstance()->AddInstance({
			effect.transform,
			effect.color
		});
	}
	hitEffect_.erase(std::remove_if(hitEffect_.begin(), hitEffect_.end(),
		[](const HitEffectInfo& effect) { return effect.currentTime >= effect.lifeTime; }),
		hitEffect_.end());
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
		info.transform.set_.Scale({ 0.05f,0.3f + 0.5f * rand->GetHighRandom().GetFloat(0.0f,1.0f),1.0f});
		info.transform.set_.Rotation({ 0.0f,180.0f, ((float)rand->GetHighRandom().GetInt(0,360)) });
		info.transform.set_.Translation({ obj_->worldTransform_.GetWorldPos().x,obj_->worldTransform_.GetWorldPos().y + 0.2f,obj_->worldTransform_.GetWorldPos().z });
		info.transform.LocalToWorld();
		info.lifeTime = 1.0f;
		info.currentTime = 0.0f;
		info.color = Vector4(1.0f, 0.5f, 0.5f, 1.0f);
		hitEffect_.push_back(info);
	}
	hitEffectCoolTimer_ = 0.2f;
}