#pragma once
#include "../IEditor.h"
#include "SceneEditorCommand.h"

class SceneEditor : public BaseEditor<DynamicObject> {
public:
    SceneEditor(SceneMap* sceneMap)
        : BaseEditor("Scene Editor"), sceneMap_(sceneMap) {}virtual ~SceneEditor() override = default;

    void Update() override;
    void DrawUI() override;

private:
    void SyncFromSceneMap() {
        if (sceneMap_) {
            // SceneMap の最新リストで targetObjects_ を更新
            SetTargetObjects(sceneMap_->GetRawObjects());
        }
    }

private:
    int selectedObjectIndex_ = -1; // 現在選択中のオブジェクトのインデックス[cite: 8, 9]

    // ImGuizmo操作用
    int gizmoOperation_ = 0; // 0: Translate, 1: Rotate, 2: Scale
    bool isGizmoUsingLastFrame_ = false; // 前フレームでギズモを触っていたか[cite: 8, 9]
    TransformState gizmoOldState_; // 触る直前のトランスフォームバックアップ[cite: 8, 9]

    // 衝突球のクリック判定半径
    float selectRadius_ = 1.0f;

    SceneMap* sceneMap_;
};