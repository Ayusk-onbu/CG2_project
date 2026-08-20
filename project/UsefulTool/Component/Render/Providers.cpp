#include "Providers.h"
#include "DynamicObject.h"
#include "../Skinning/SkinningComponent.h"

void GrassRenderProvider::SendToDrawManager(DynamicObject* owner){
    data.worldTransform = owner->GetTransform();
    DrawManager::GetInstance()->GetGrass()->AddInstance(data);
}

void GrassRenderProvider::DrawUI() {
#ifdef USE_IMGUI
    ImGui::Text("Provider: Grass Render");

    auto* texMgr = TextureManager::GetInstance();

    // 1. 現在設定されている Bindless ID (SRV Index) からテクスチャ名を取得
    // ※ data 側で Bindless ID を保持している変数名（例: data.textureIndex ）に合わせて変更してください
    std::string currentTexName = texMgr->GetTextureNameBySrvIndex(data.textureIndex);

    // 2. ImGui Combo (ドロップダウン) で TextureManager 内のテクスチャ一覧を表示
    if (ImGui::BeginCombo("Grass Texture", currentTexName.c_str())) {

        for (const auto& [name, tex] : texMgr->GetData()) {
            if (!tex) continue;

            bool isSelected = (currentTexName == name);

            // テクスチャ名とSRVインデックスを一覧表示
            std::string label = name + " (ID: " + std::to_string(tex->GetSrvIndex()) + ")";

            if (ImGui::Selectable(label.c_str(), isSelected)) {
                // 選択したテクスチャの Bindless ID (SRV Index) を GrassObjectData にセット！
                data.textureIndex = tex->GetSrvIndex();
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 現在適用されている Bindless ID を数値でデバッグ表示
    ImGui::Text("Applied SRV Index: %u", data.textureIndex);

    // GrassObjectData 内にカラーやパラメータがあればここで編集
    // 例: カラー編集（Vector4 や Float4 の場合）
    ImGui::ColorEdit4("Grass Color", &data.color.x);

    // 追加のパラメータ例（草の揺れ速度、サイズ、テクスチャIDなど）
    ImGui::DragFloat("Wind Scale", &data.windPhase, 0.01f);
#endif
}

void CharacterRenderProvider::SendToDrawManager(DynamicObject* owner) {
    if (!owner) return;

    // ワールドトランスフォームを設定
    data.worldTransform = owner->GetTransform();

    // SkinningComponent が存在するかチェック
    if (auto* skinning = owner->GetComponent<SkinningComponent>()) {
        data.modelName = skinning->GetModelID();
        // GPU スキニング後の最新 VBV をセット！
        data.vertexBufferView = skinning->GetOutputVertexBufferView();
        data.useCustomVB = true;
    }
    else {
        // スキニングがない場合は静的メッシュの VBV をセット（またはデフォルトフラグ）
        data.useCustomVB = false;
    }

    // DrawManager 経由で PrimitiveCharacter に登録！
    DrawManager::GetInstance()->GetCharacter()->AddInstance(data);
}

void CharacterRenderProvider::DrawUI() {
#ifdef USE_IMGUI
    if (ImGui::TreeNode("Character Render Settings")) {
        ImGui::Text("Skinning Mode: %s", data.useCustomVB ? "Active (GPU Skinned)" : "Static Mesh");

        // カラーピッカー
        ImGui::ColorEdit4("Color", &data.color.x);

        ImGui::TreePop();
    }
#endif
}