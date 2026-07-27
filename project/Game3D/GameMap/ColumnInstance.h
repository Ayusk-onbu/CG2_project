#pragma once
#include "WorldTransform.h"
#include <string>

// エディタで操作する「柱1本」を表すクラス
class ColumnInstance {
public:
    ColumnInstance() {
        transform_.Initialize();
        uvTransform_.Initialize();
        color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    // 更新処理（行列の計算など）
    void Update();

#ifdef _DEBUG
    // エディタ（ImGui）用の編集画面を表示する関数
    void DrawImGui(int id);
#endif

public:
    // マネージャーがデータを回収するために公開しておく（またはGetterを作る）
    WorldTransform transform_;
    WorldTransform uvTransform_;
    Vector4 color_;
    std::string colorTextureName = "Tiles083_1K-PNG_Color";
    std::string normalTextureName = "Tiles083_1K-PNG_NormalDX";

    bool isDead_ = false; // エディタで「削除」されたときにtrueにするフラグ
};