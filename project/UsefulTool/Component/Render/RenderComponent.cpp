#include "RenderComponent.h"
#include "DynamicObject.h"

void RenderComponent::Update(float deltaTime) {
    if (!master_ || !provider_) return;
    
    master_->GetTransform().LocalToWorld();
    provider_->SendToDrawManager(master_);
}

void RenderComponent::DrawUI() {
#ifdef USE_IMGUI
    ImGui::Text("Render Provider Selection");

    // プロバイダー切り替え UI
    const char* providers[] = { "Character", "Grass" };
    static int currentIdx = 0;

    if (ImGui::Combo("Provider Type", &currentIdx, providers, IM_ARRAYSIZE(providers))) {
        if (currentIdx == 0) SetProvider<CharacterRenderProvider>();
        else if (currentIdx == 1) SetProvider<GrassRenderProvider>();
    }

    ImGui::Separator();

    // 現在保持している Provider 固有の UI を描画
    if (provider_) {
        provider_->DrawUI();
    }
    else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No RenderProvider Attached!");
        return;
    }
#endif
}