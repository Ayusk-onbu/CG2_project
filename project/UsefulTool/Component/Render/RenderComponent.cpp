#include "RenderComponent.h"
#include "DynamicObject.h"

void RenderComponent::Update(float deltaTime) {
    if (master_ && provider_) {
        master_->GetTransform().LocalToWorld();
        provider_->SendToDrawManager(master_->GetTransform());
    }
}

void RenderComponent::DrawUI() {
#ifdef USE_IMGUI
    ImGui::Text("Render Provider Selection");

    // --- プロバイダー切替ボタン ---
    if (ImGui::Button("Set GrassRender")) {
        SetProvider<GrassRenderProvider>();
    }

    /* 将来的に別の描画プロバイダー（例: ModelRenderProvider）が増えたらここに追加
    ImGui::SameLine();
    if (ImGui::Button("Set ModelRender")) {
        SetProvider<ModelRenderProvider>();
    }
    */

    if (!provider_) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No RenderProvider Attached!");
        return;
    }

    ImGui::Separator();

    // 保持しているプロバイダー独自の UI を呼び出す
    provider_->DrawUI();
#endif
}