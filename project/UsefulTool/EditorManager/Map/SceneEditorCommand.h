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

class TestCommand : public ICommand
{
public:
    TestCommand(){}
public:
    void Execute()override {
        Log::View("Test Birth");
    }
    void Undo()override {
        Log::View("Test Break");
    }
    void Redo()override {
        Execute();
    }
};

class TestCommandND : public ICommand
{
public:
    TestCommandND() {}
public:
    void Execute()override {
        Log::View("Test Birth My Baby");
    }
    void Undo()override {
        Log::View("Test Break My Baby");
    }
    void Redo()override {
        Execute();
    }
};