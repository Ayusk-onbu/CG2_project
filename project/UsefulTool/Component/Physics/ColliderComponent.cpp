#include "ColliderComponent.h"
#include "DynamicObject.h"
#include "CollisionManager.h"
#include "CollisionLayerManager.h"

void ColliderComponent::Initialize() {
    if (collider_ && master_) {
        collider_->SetUserData(master_);

        // C++ のコールバックが発火したら、親（DynamicObject）に丸投げする！
        collider_->onCollisionCallBack = [this](Collider* other, const Vector3& pushOut) {
            if (!master_) return;
            auto* otherObject = static_cast<DynamicObject*>(other->GetUserData());
            master_->OnCollision(otherObject, pushOut);
        };
    }
}

void ColliderComponent::Update(float deltaTime) {
    if (!master_ || !collider_) return;

    // UserData が空なら親をセット
    if (!collider_->GetUserData()) {
        collider_->SetUserData(master_);
    }

    // 親の WorldTransform に合わせて判定用の座標/行列を更新
    if (auto* meshCol = dynamic_cast<MeshCollider*>(collider_.get())) {
        meshCol->SetWorldMatrix(master_->GetTransform().mat_);
        meshCol->Update();
    }
    else {
        // 通常の球やAABBコライダーは位置情報を同期
        collider_->SetWorldPosition(master_->GetTransform().get_.Translation());
    }

    // 3. このフレームの判定用リストへ自動登録！
    CollisionManager::GetInstance()->SetColliders(collider_.get());
}

void ColliderComponent::SetLayer(const std::string& layerName) {
    if (collider_) {
        uint32_t bit = CollisionLayerManager::GetInstance()->GetBitByName(layerName);
        collider_->SetMyType(bit);
    }
}

void ColliderComponent::SetTargetLayers(const std::vector<std::string>& targetLayerNames) {
    if (collider_) {
        uint32_t mask = 0;
        for (const auto& name : targetLayerNames) {
            mask |= CollisionLayerManager::GetInstance()->GetBitByName(name);
        }
        collider_->SetYourType(mask);
    }
}