#pragma once
#include "WorldTransform.h"
#include "ModelObject.h"
#include "Texture.h"
#include "CameraBase.h"


class PlayerBullet {
public:
	void Initialize(D3D12System& d3d12, ModelObject* model, const Vector3& position,const Vector3& velocity);
	void Update();
	void Draw(CameraBase& camera,
		TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);

	bool IsDead()const { return isDead_; }
private:
	ModelObject model_;
	WorldTransform worldTransform_;
	Vector3 velocity_;

	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;
};

class Player
{
public:
	~Player();
public:
	void Initialize(D3D12System& d3d12, std::unique_ptr<ModelObject>model, CameraBase* camera);
	void GetBullet(ModelObject* model,Texture*texture);
	void Update();
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
private:
	void Move(Vector3& pos);
	void Rotate(Vector3& rotation);
	void Attack();
private:
	D3D12System* d3d12_;

	std::unique_ptr<ModelObject>model_;
	CameraBase* camera_;
	WorldTransform worldTransform_;

	std::list<PlayerBullet*> bullets_;
	ModelObject* bulletModel_ = nullptr;
	Texture* bulletTex_ = nullptr;

	const float kMoveSpeed_ = 0.1f;
	const Vector2 kMoveLimitX_ = { 15.0f,8.5f };
	const float kRotSpeed_ = 1.0f;//Deg
};

