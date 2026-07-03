#pragma once
#include "ICommand.h"
#include "Hair/GuideCurve.h"

class Hair;

class HairGuideMoveCommand : public ICommand
{
private:
    Hair* hairSystem_;                                        // GPUバッファ更新用の参照
    std::vector<int> pointIndex_;
    std::vector<Vector3> oldPosition_;
    std::vector<Vector3> newPosition_;                     // 移動後の座標リスト

public:
    HairGuideMoveCommand(Hair* hair, const std::vector<int>& pointIndices, const std::vector<Vector3>& oldPositions, const std::vector<Vector3>& newPositions)
        : hairSystem_(hair), pointIndex_(pointIndices), oldPosition_(oldPositions), newPosition_(newPositions) {
    }

    void Execute() override {
        auto* cpuData = hairSystem_->GetCPUGuideData();
        for (size_t i = 0; i < pointIndex_.size(); ++i) {
            cpuData[pointIndex_[i]].position = newPosition_[i];
            cpuData[pointIndex_[i]].homePosition = newPosition_[i];
        }
        hairSystem_->RequestNotifyUpdate(); // GPUに通知
    }

    void Undo() override {
        auto* cpuData = hairSystem_->GetCPUGuideData();
        for (size_t i = 0; i < pointIndex_.size(); ++i) {
            cpuData[pointIndex_[i]].position = oldPosition_[i];
            cpuData[pointIndex_[i]].homePosition = oldPosition_[i];
        }
        hairSystem_->RequestNotifyUpdate(); // GPUに通知
    }

    void Redo() override {
        Execute();
    }
};