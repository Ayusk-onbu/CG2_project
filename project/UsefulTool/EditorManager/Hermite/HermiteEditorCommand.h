#pragma once
#include "ICommand.h"
#include "Structures.h"
#include <vector>
import Hermite;

using SplineNode = MathUtils::Spline::Node<Vector3>;
using NodePtrVector = std::vector<SplineNode*>;

// =================================================================
// 1. 値の編集コマンド
// =================================================================
class HermiteEditorCommand : public ICommand {
public:
    HermiteEditorCommand(NodePtrVector* nodes, int index, SplineNode oldState, SplineNode newState)
        : nodes_(nodes), index_(index), oldState_(oldState), newState_(newState) {
    }

    void Execute() override {
        *(*nodes_)[index_] = newState_; // ポインタの「指す先の実体」を書き換える
    }
    void Undo() override {
        *(*nodes_)[index_] = oldState_;
    }
    void Redo() override { Execute(); }

private:
    NodePtrVector* nodes_;
    int index_;
    SplineNode oldState_;
    SplineNode newState_;
};

// =================================================================
// 2. ノード追加コマンド（メモリの new / delete を安全に行う）
// =================================================================
class SplineNodeAddCommand : public ICommand {
public:
    SplineNodeAddCommand(NodePtrVector* nodes, SplineNode nodeData)
        : nodes_(nodes), nodeData_(nodeData) {
    }

    void Execute() override {
        // 生ポインタの配列なので、new して追加する
        nodes_->push_back(new SplineNode(nodeData_));
    }
    void Undo() override {
        // Undoされたら、メモリリークしないように delete してから取り除く
        delete nodes_->back();
        nodes_->pop_back();
    }
    void Redo() override { Execute(); }

private:
    NodePtrVector* nodes_;
    SplineNode nodeData_;
};

// =================================================================
// 3. ノード削除コマンド（メモリの delete / new を安全に行う）
// =================================================================
class SplineNodeDeleteCommand : public ICommand {
public:
    // 消す前に、ポインタの指す先の実体をコピーしてバックアップしておく
    SplineNodeDeleteCommand(NodePtrVector* nodes, int index)
        : nodes_(nodes), index_(index), removedNode_(*(*nodes)[index]) {
    }

    void Execute() override {
        delete (*nodes_)[index_]; // メモリリーク防止のために delete
        nodes_->erase(nodes_->begin() + index_);
    }
    void Undo() override {
        // Undoされたら、バックアップから new し直して元の位置に挿入する
        nodes_->insert(nodes_->begin() + index_, new SplineNode(removedNode_));
    }
    void Redo() override { Execute(); }

private:
    NodePtrVector* nodes_;
    int index_;
    SplineNode removedNode_;
};