#include "ColumnInstance.h"
#include "DrawManager.h"

void ColumnInstance::Update() {
    DrawImGui(0);
    transform_.LocalToWorld();
    uvTransform_.LocalToWorld();

    ColumnObjectData data;
    data.worldTransform = transform_;
    data.uvTransform = uvTransform_;
    data.color = color_;
    data.colorTextureName = colorTextureName;
    data.normalTextureName = normalTextureName;

    DrawManager::GetInstance()->GetColumn()->AddInstance(data);
}

void ColumnInstance::DrawImGui(int id) {
    {
        std::string label = "Column_" + std::to_string(id);
        if (ImGui::TreeNode(label.c_str())) {
            // 位置・回転・スケールの変更
            ImGui::DragFloat3("Position", &transform_.transform_.translation_.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform_.transform_.rotation_.x, 0.05f);
            ImGui::DragFloat3("Scale", &transform_.transform_.scale_.x, 0.1f);
            transform_.isDirty_ = true;

            ImGui::DragFloat3("UVPosition", &uvTransform_.transform_.translation_.x, 0.1f);
            ImGui::DragFloat3("UVRotation", &uvTransform_.transform_.rotation_.x, 0.05f);
            ImGui::DragFloat3("UVScale", &uvTransform_.transform_.scale_.x, 0.1f);
            uvTransform_.isDirty_ = true;

            // 色の変更
            ImGui::ColorEdit4("Color", &color_.x);

            ImGui::TreePop();
        }
    }
}