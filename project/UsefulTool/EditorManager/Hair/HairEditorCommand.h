#pragma once
#include "ICommand.h"
#include "Hair/GuideCurve.h"

class Hair;

class HairGuideMoveCommand : public ICommand
{
private:
    Hair* hairSystem_;                                        // GPUバッファ更新用の参照
    int pointIndex_;
    Vector3 oldPosition_;
    Vector3 newPosition_;                     // 移動後の座標リスト

public:
    HairGuideMoveCommand(Hair* hair, int pointIndex, Vector3 oldPos, Vector3 newPos)
        : hairSystem_(hair), pointIndex_(pointIndex), oldPosition_(oldPos), newPosition_(newPos) {
    }

    void Execute() override {
        auto* cpuData = hairSystem_->GetCPUGuideData();
        cpuData[pointIndex_].position = newPosition_;
        cpuData[pointIndex_].homePosition = newPosition_;
        hairSystem_->RequestNotifyUpdate(); // GPUに通知
    }

    void Undo() override {
        auto* cpuData = hairSystem_->GetCPUGuideData();
        cpuData[pointIndex_].position = oldPosition_;
        cpuData[pointIndex_].homePosition = newPosition_;
        hairSystem_->RequestNotifyUpdate(); // GPUに通知
    }

    void Redo() override {
        Execute();
    }
};