#pragma once
#include "../IEditor.h"
#include "HermiteEditorCommand.h"

class HermiteEditor : public BaseEditor<MathUtils::Spline::Node<Vector3>> {
public:
    HermiteEditor() : BaseEditor("Hermite Editor")
                    ,gizmoOldState_({0.0f,0.0f,0.0f}) {}

    void Update() override;

    void DrawUI() override;
private:
    Vector3 scale_ = {0.01f,0.01f,0.01f};
    int selectedNodeIndex_ = -1;       // 現在ギズモ操作対象に選ばれているノード番号 (-1は未選択)
    int selectedElement_ = 0;          // 0: Position, 1: TangentIn, 2: TangentOut
    bool isBroken_ = false;             // 操作を連動させるか

    bool isGizmoUsingLastFrame_ = false; // 前のフレームでギズモを触っていたか（確定検知用）
    MathUtils::Spline::Node<Vector3> gizmoOldState_; // ギズモを触る直前の状態のバックアップ
};