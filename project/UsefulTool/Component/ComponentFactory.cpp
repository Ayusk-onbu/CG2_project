#include "ComponentFactory.h"
#include "Render/RenderComponent.h"

void ComponentFactory::Initialize() {
    // --------------------------------------------------
    // 描画系コンポーネントの登録
    // --------------------------------------------------
    Register("GrassRender", []() {
        auto comp = std::make_unique<RenderComponent>();
        comp->SetProvider<GrassRenderProvider>();
        return comp;
        });

    /*Register("SphereRender", []() {
        auto comp = std::make_unique<RenderComponent>();
        comp->SetProvider<SphereRenderProvider>();
        return comp;
        });*/

    // --------------------------------------------------
    // 物理・ロジック系コンポーネントの登録
    // --------------------------------------------------
    /*
    Register("RigidBody", []() {
        return std::make_unique<RigidBodyComponent>();
    });
    */
}

std::unique_ptr<Component> ComponentFactory::Create(const std::string& name) {
    auto it = registry_.find(name);
    if (it != registry_.end()) {
        return it->second(); // 登録された生成処理（ラムダ）を実行して返す！
    }

    // 見つからなかった場合
    return nullptr;
}