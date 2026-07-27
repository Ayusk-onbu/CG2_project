#pragma once
#include "Component.h"
#include "Collider.h"

class ColliderComponent : public Component {
public:
    ColliderComponent() = default;
    virtual ~ColliderComponent() = default;

    // --- コンポーネントの基本処理 ---
    void Initialize() override;
    void Update(float deltaTime) override;

    // --- コライダーの生成・セット ---
    template <typename T, typename... Args>
    T* SetCollider(Args&&... args) {
        auto collider = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = collider.get();
        collider_ = std::move(collider);

        // 属性やコールバックの初期設定を適用
        if (collider_) {
            collider_->SetMyType(attribute_);
                collider_->SetYourType(mask_);
                collider_->onCollisionCallBack = callback_;
        }
        return ptr;
    }

    Collider* GetCollider() const { return collider_.get(); }

    // レイヤー名（文字列）で衝突属性・マスクを設定
    void SetLayer(const std::string& layerName);
    void SetTargetLayers(const std::vector<std::string>& targetLayerNames);

private:
    std::unique_ptr<Collider> collider_;

    uint32_t attribute_ = 0xffffffff;
    uint32_t mask_ = 0xffffffff;
    Collider::CollisionCallBack callback_ = nullptr;
};