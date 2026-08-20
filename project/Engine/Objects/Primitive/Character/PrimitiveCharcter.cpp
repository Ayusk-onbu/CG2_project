#include "PrimitiveCharcter.h"
#include "ModelManager.h"
#include "SRVManager.h"

void PrimitiveCharacter::Initialize(Fngine* engine, uint32_t numInstance) {
    PrimitiveBaseModel::Initialize(engine, numInstance);
    useModelName_ = "Naira_ExportTest";

    SetCommand = [this]() {
        if (instanceDataList_.empty()) return;

        auto commandList = p_engine_->GetCommand().GetList().GetList();
        
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Character").GetRootSignature().GetRS().Get());
        commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Character").GetGPS().Get());

        // 記述子ヒープ等のバインド
        commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());
        commandList->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetHeap()->GetGPUDescriptorHandleForHeapStart());

        // 各インスタンスを描画
        for (UINT i = 0; i < static_cast<UINT>(instanceDataList_.size()); ++i) {
            const auto& instanceData = instanceDataList_[i];

            // 受け取った ModelID を使って ModelManager から静的データを取得
            auto& objData = ModelManager::GetInstance()->LoadObjectData(instanceData.modelName);

            // 頂点バッファ(VBV)のセット (スキニング動的VBV or 静的VBV)
            if (instanceData.useCustomVB) {
                // Dispachしてないからデータが存在しない
                commandList->IASetVertexBuffers(0, 1, &instanceData.vertexBufferView);
                //commandList->IASetVertexBuffers(0, 1, &objData.GetVertexBufferView());
            }
            else {
                commandList->IASetVertexBuffers(0, 1, &objData.GetVertexBufferView());
            }

            // ModelManager から取得した IBV と IndexCount をセットして描画！
            commandList->IASetIndexBuffer(&objData.GetIndexBufferView());
            commandList->DrawIndexedInstanced(objData.GetIndexCount(), 1, 0, 0, i);
        }
    };
}

CharacterForGPU PrimitiveCharacter::ConvertToGPUData(const CharacterObjectData& data) {
    CharacterForGPU returnData{};
    returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldTransform.mat_);
    returnData.World = data.worldTransform.mat_;
    returnData.uvTransform = data.uvTransform.mat_;
    returnData.color = data.color;

    // テクスチャインデックスを取得
    returnData.colorTextureIndex = TextureManager::GetInstance()->GetTexture(data.colorTextureName).GetSrvIndex();
    returnData.normalTextureIndex = TextureManager::GetInstance()->GetTexture(data.normalTextureName).GetSrvIndex();

    return returnData;
}