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

class AddGuideCommand : public ICommand {
private:
    Hair* hairSystem_;
    int targetGuideIndex_;
    GuideCurve::GuideInfo oldInfo_; // Undo用（基本はvertexCount=0の空データ）
    GuideCurve::GuideInfo newInfo_; // Redo用
    std::vector<GuideCurve::ControllerPoint> generatedPoints_; // 生成された頂点データ

public:
    AddGuideCommand(Hair* hairSystem, int targetGuideIndex,
        const GuideCurve::GuideInfo& oldInfo,
        const GuideCurve::GuideInfo& newInfo,
        const std::vector<GuideCurve::ControllerPoint>& generatedPoints)
        : hairSystem_(hairSystem), targetGuideIndex_(targetGuideIndex),
        oldInfo_(oldInfo), newInfo_(newInfo), generatedPoints_(generatedPoints) {
    }

    void Execute() override {
        Redo(); // 初回実行はRedoと同じ処理
    }

    void Undo() override {
        // Undo時は対象のガイド情報を元に戻す（無効化する）
        auto* guideInfoData = hairSystem_->GetCPUGuideInfoData();
        guideInfoData[targetGuideIndex_] = oldInfo_;
        hairSystem_->RequestNotifyUpdate();
    }

    void Redo() override {
        // Redo時はガイド情報と頂点データを再度書き込む
        auto* guideInfoData = hairSystem_->GetCPUGuideInfoData();
        auto* cpuData = hairSystem_->GetCPUGuideData();

        // 頂点データの復元
        for (size_t i = 0; i < generatedPoints_.size(); ++i) {
            cpuData[newInfo_.vertexStartIndex + i] = generatedPoints_[i];
        }

        // ガイド情報の復元
        guideInfoData[targetGuideIndex_] = newInfo_;
        hairSystem_->RequestNotifyUpdate();
    }
};

class AddChildStrandCommand : public ICommand {
private:
    Hair* hairSystem_;
    int targetStrandIndex_;

    // 復元用データ
    Strands::StrandInfo oldInfo_;

    // 書き込み用データ
    Strands::StrandInfo newInfo_;
    Strands::ChildStrand newChild_;
    std::vector<Strands::SegmentData> newSegments_;

public:
    AddChildStrandCommand(Hair* hairSystem,
        int targetStrandIndex,
        const Strands::StrandInfo& oldInfo,
        const Strands::StrandInfo& newInfo,
        const Strands::ChildStrand& newChild,
        const std::vector<Strands::SegmentData>& newSegments)
        : hairSystem_(hairSystem),
        targetStrandIndex_(targetStrandIndex),
        oldInfo_(oldInfo),
        newInfo_(newInfo),
        newChild_(newChild),
        newSegments_(newSegments) {
    }

    void Execute() override {
        Redo(); // 初回実行はRedoと同一処理
    }

    void Undo() override {
        // 対象のStrandInfoを元（無効な状態）に戻す
        auto* strandInfoData = hairSystem_->GetCPUStrandInfoData();
        strandInfoData[targetStrandIndex_] = oldInfo_;

        hairSystem_->RequestNotifyUpdate();
    }

    void Redo() override {
        auto* strandInfoData = hairSystem_->GetCPUStrandInfoData();
        auto* childStrandData = hairSystem_->GetCPUChildStrandData();
        auto* segmentData = hairSystem_->GetCPUSegmentData();

        // StrandInfoとChildStrandの書き込み
        strandInfoData[targetStrandIndex_] = newInfo_;
        childStrandData[targetStrandIndex_] = newChild_;

        // SegmentDataの書き込み
        uint32_t segmentCount = newInfo_.vertexCount - 1;
        for (uint32_t j = 0; j < segmentCount; ++j) {
            uint32_t globalAABBIndex = newInfo_.aabbStartIndex + j;
            segmentData[globalAABBIndex] = newSegments_[j];
        }

        hairSystem_->RequestNotifyUpdate();
    }
};