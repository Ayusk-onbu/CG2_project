#pragma once
#include "Bullet.h" // Bullet.h が必要
#include <list>

class BulletManager final
{
public:
    void SetFngine(Fngine* fngine) { fngine_ = fngine; }
    // 💡 BossState から呼ばれるメソッド
    void CreateBullet(const Vector3& startPos,const Vector3& velocity,uint32_t myType,uint32_t yourType);

    void Update();
    void Draw();
    void SetRadius(float rad) { radius_ = rad; }
    void SetLifeTime(float time) { lifeTime_ = time; }

    std::list<std::unique_ptr<Bullet>>bullets_;
private:
    Fngine* fngine_;
    float lifeTime_ = 2.0f;
    float radius_ = 0.3f;
};