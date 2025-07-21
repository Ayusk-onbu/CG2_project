#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"
#include "EnemyState.h"
#include "TimedCall.h"

class Player;

class EnemyBullet {
public:
	void Initialize(D3D12System& d3d12, ModelObject* model, const Vector3& position, const Vector3& velocity);
	void Update();
	void Draw(CameraBase& camera,
		TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	void Homing();
	void OnCollision();
	void SetTarget(const Player& player);
	bool IsDead()const { return isDead_; }
	const WorldTransform& GetWorldTransform()const { return worldTransform_; }
private:
	ModelObject model_;
	WorldTransform worldTransform_;
	Vector3 velocity_;
	const Player* player_;

	float bulletSpeed_;

	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;
};

class Enemy
{
public:
	/*enum class Phase {
		Approach,
		Leave,
	};*/
	Enemy();
	~Enemy();

	void Initialize(D3D12System& d3d12, ModelObject& model, CameraBase* camera,const Vector3&pos);
	void SetBullet(ModelObject* model, Texture* texture);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void ChangeState(EnemyState* state);
	void Fire();
	void FireReset();
	void SetTarget(const Player& player);
	const WorldTransform& GetWorldTransform()const { return worldTransform_; }

	void OnCollision();

	const std::list<EnemyBullet*>& GetBullets()const { return bullets_; }
private:
	/*void ApproachUpdate(Vector3& pos, Vector3& rotation);
	void ApproachMove(Vector3& pos);
	void ApproachRotate(Vector3& rotation);

	void LeaveUpdate(Vector3& pos, Vector3& rotation);
	void LeaveMove(Vector3& pos);
	void LeaveRotate(Vector3& rotation);*/
private:
	//static void (Enemy::*pFuncTable[])(Vector3& pos, Vector3& rotation);
	EnemyState* state_;
private:
	D3D12System* d3d12_;

	ModelObject model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;

	std::list<EnemyBullet*>bullets_;
	ModelObject* bulletModel_;
	Texture* bulletTex_;
	const Player* player_;
	const float kFireTime_ = 10.0f;
	//float fireTimer_ = kFireTime_;
	std::list<TimedCall*>timedCalls_;
	//Phase phase_ = Phase::Approach;
	//const float kMoveSpeed_ = 0.1f;
	//const Vector3 kLeaveSpeed_ = {-0.05f,0.05f,0.1f};
};

