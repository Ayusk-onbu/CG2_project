#pragma once
#include "Engine/Objects/DrawManager.h"
#include "TextureManager.h"
#include "WorldTransform.h"

class DynamicObject;

// --- 描画データのインターフェース ---
class IRenderProvider {
public:
    virtual ~IRenderProvider() = default;
    virtual void SendToDrawManager(const WorldTransform& transform) = 0;
    virtual void DrawUI() {}
};

class GrassRenderProvider : public IRenderProvider {
public:
    GrassObjectData data;

    void SendToDrawManager(const WorldTransform& transform) override {
        data.worldTransform = transform;
        DrawManager::GetInstance()->GetGrass()->AddInstance(data);
    }

    void DrawUI() override {
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
};