#pragma once
#include "Character.h"
#include "Animation.h"

struct HitEffectInfo {
	WorldTransform transform;
	float lifeTime;
	Vector4 color;
	float currentTime;
};

class Player : public Character
{
public:
	Player();
	~Player()override = default;

	void TakeDamage();

///////////////////////////
/// 
/// 基本的な存在
///
//////////////////////////
public:
	void Initialize(Fngine* fngine);
	void Update(float deltaTime)override;
	void Draw()override;

private:
	std::unique_ptr<Skeleton>skeleton_ = nullptr;

	std::unique_ptr<Animation>animation_ = nullptr;

///////////////////////////
/// 
/// 物理的な存在達
///
//////////////////////////
public:


private:
	
///////////////////////////
/// 
/// 一時的なParticleの存在
///
//////////////////////////
public:
	void MakeHitEffect();

private:
	std::vector<HitEffectInfo>hitEffect_;
	float hitEffectCoolTimer_ = 0.0f;

public:
	// ビームCollider
	std::unique_ptr<MeshCollider>beamCollider_ = nullptr;

///////////////////////////
/// 
/// 髪のための関数
///
//////////////////////////
public:
	Matrix4x4 GetHeadMatrix()const;
	Vector3 GetHeadPos()const;
	void SetHeadPosToCameraTarget();
};
