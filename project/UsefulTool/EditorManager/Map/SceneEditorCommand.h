#pragma once
#include "ICommand.h"
#include "InGame/Map/SceneMap.h"

using ObjectPtrVector = std::vector<DynamicObject*>;

// トランスフォーム保存用の簡易構造体
struct TransformState {
    Vector3 position{ 0,0,0 };
    Vector3 rotation{ 0,0,0 };
    Vector3 scale{ 1,1,1 };
};

// =================================================================
// 1. Transform変更コマンド（位置・回転・スケールのUndo/Redo）
// =================================================================
class DynamicObjectTransformCommand : public ICommand {
public:
    DynamicObjectTransformCommand(DynamicObject* obj, TransformState oldState, TransformState newState)
        : obj_(obj), oldState_(oldState), newState_(newState) {
    }

    void Execute() override {
        ApplyState(newState_);
    }
    void Undo() override {
        ApplyState(oldState_);
    }
    void Redo() override { Execute(); }

private:
    void ApplyState(const TransformState& state) {
        if (!obj_) return;
        obj_->SetPosition(state.position);
        obj_->SetRotation(state.rotation);
        obj_->SetScale(state.scale);
    }

private:
    DynamicObject* obj_;
    TransformState oldState_;
    TransformState newState_;
};

// =================================================================
// 2. オブジェクト追加コマンド
// =================================================================
class SceneObjectAddCommand : public ICommand {
public:
    SceneObjectAddCommand(ObjectPtrVector* objects, DynamicObject* newObj)
        : objects_(objects), createdObj_(newObj) {
    }

    void Execute() override {
        objects_->push_back(createdObj_);
    }
    void Undo() override {
        // 配列から取り除く（メモリは保持してRedoに備える）
        std::erase(*objects_, createdObj_);
    }
    void Redo() override { Execute(); }

private:
    ObjectPtrVector* objects_;
    DynamicObject* createdObj_;
};

// =================================================================
// 3. オブジェクト削除コマンド
// =================================================================
class SceneObjectDeleteCommand : public ICommand {
public:
    SceneObjectDeleteCommand(ObjectPtrVector* objects, int index)
        : objects_(objects), index_(index) {
        if (index >= 0 && index < (int)objects->size()) {
            removedObj_ = (*objects)[index];
        }
    }

    void Execute() override {
        if (index_ >= 0 && index_ < (int)objects_->size()) {
            objects_->erase(objects_->begin() + index_);
        }
    }
    void Undo() override {
        // 元の位置に復元
        objects_->insert(objects_->begin() + index_, removedObj_);
    }
    void Redo() override { Execute(); }

private:
    ObjectPtrVector* objects_;
    int index_;
    DynamicObject* removedObj_ = nullptr;
};