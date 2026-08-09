#pragma once
#include "InputHandler.h"
#include <string>

class DynamicObject;

class IMovementAction {
public:
    virtual ~IMovementAction() = default;

    // --- 発動条件の判定 (Behavior Tree の Selector ノード相当) ---
    // 現在の入力、物理状態、スタミナなどを参照して実行可能かを返す
    virtual bool CanExecute(DynamicObject* owner, const CommandState& input) const = 0;

    // --- ライフサイクル ---
    virtual void OnEnter(DynamicObject* owner) {}
    virtual void OnUpdate(DynamicObject* owner, const CommandState& input, float deltaTime) = 0;
    virtual void OnExit(DynamicObject* owner) {}

    // --- 設定値 ---
    virtual int GetPriority() const = 0; // 数値が高いほど優先される (例: Walk=10, Jump=50)
    virtual const std::string& GetName() const = 0; // デバッグ / ImGui 表示用
};