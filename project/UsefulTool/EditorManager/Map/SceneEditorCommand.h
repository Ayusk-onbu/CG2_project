#pragma once
#include "ICommand.h"
#include "../../../InGame/DynamicObject/DynamicObject.h"

class DynamicObjectMoveCommand : public ICommand
{
public:
    DynamicObjectMoveCommand(DynamicObject* obj,Vector3 moveAmount) : obj_(obj), moveAmount_(moveAmount) {}
    // 基本情報
public:
    void Execute()override {
        // 移動
        obj_->GetObj()->worldTransform_.set_.Translation(obj_->GetObj()->worldTransform_.get_.Translation() + moveAmount_);
    }
    void Undo()override {
        obj_->GetObj()->worldTransform_.set_.Translation(obj_->GetObj()->worldTransform_.get_.Translation() - moveAmount_);
    }
    void Redo()override {
        Execute();
    }
private:
    Vector3 moveAmount_;
    DynamicObject* obj_;
};