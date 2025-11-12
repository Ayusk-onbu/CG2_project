#include "BulletManager.h"

void BulletManager::CreateBullet(const Vector3& startPos, const Vector3& velocity, uint32_t myType, uint32_t yourType) {

    // 弾の生成
    std::unique_ptr<Bullet>newBullet = std::make_unique<Bullet>();

    newBullet->Initialize(fngine_, startPos, velocity, myType, yourType, radius_, lifeTime_);

    bullets_.push_back(std::move(newBullet));
}

void BulletManager::Update() {
    auto it = bullets_.begin();
    while (it != bullets_.end()) {
        Bullet* bullet = it->get();

        // 1. Bulletの更新（移動・寿命チェック）
        bullet->Update();

        // 2. 死亡判定
        if (bullet->IsDead()) {

            // 死亡したBulletをリストから削除
            // unique_ptr なので、リストから削除されると自動でデストラクタが呼ばれメモリ解放される
            it = bullets_.erase(it);
        }
        else {
            // 削除されなかった場合はイテレータを進める
            ++it;
        }
    }
}

void BulletManager::Draw() {
    for (const auto& bullet : bullets_) {
        bullet->Draw();
    }
}