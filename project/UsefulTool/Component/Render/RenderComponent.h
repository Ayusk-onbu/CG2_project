#pragma once
#include "Component.h"
#include "Providers.h"

// --- 共通RenderComponent ---
class RenderComponent : public Component {
public:
    template <typename T, typename... Args>
    T* SetProvider(Args&&... args) {
        auto provider = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = provider.get();
        provider_ = std::move(provider);
        return ptr;
    }
    void Update(float deltaTime)override;
    void DrawUI()override;
    /*void Draw() override;*/

private:
    std::unique_ptr<IRenderProvider> provider_;
};