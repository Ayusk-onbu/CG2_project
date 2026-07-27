#include "Column.h"
#include "ModelManager.h"
#include "SRVManager.h"

void Column::Initialize(Fngine* engine, uint32_t numInstance) {
	PrimitiveBaseModel::Initialize(engine, numInstance);
	useModelName_ = "Column"; // 使用する柱の3Dモデル名

	SetCommand = [this]() {
		auto commandList = p_engine_->GetCommand().GetList().GetList();
		auto object = ModelManager::GetInstance()->LoadObjectData(useModelName_);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Column").GetRootSignature().GetRS().Get());
		commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Column").GetGPS().Get());
		commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());
		commandList->IASetIndexBuffer(&object.GetIndexBufferView());

		// 0番: インスタンスデータ
		commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());
		// 1番: バインドレスSRVヒープの先頭
		commandList->SetGraphicsRootDescriptorTable(1, SRVManager::GetInstance()->GetHeap()->GetGPUDescriptorHandleForHeapStart());

		commandList->DrawIndexedInstanced(object.GetIndexCount(), UINT(instanceDataList_.size()), 0, 0, 0);
	};
}

ColumnForGPU Column::ConvertToGPUData(const ColumnObjectData& data) {
	ColumnForGPU returnData{};
	returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldTransform.mat_);
	returnData.World = data.worldTransform.mat_;
	returnData.uvTransform = data.uvTransform.mat_;
	returnData.color = data.color;

	// 2つのテクスチャインデックスを取得
	returnData.colorTextureIndex = TextureManager::GetInstance()->GetTexture(data.colorTextureName).GetSrvIndex();
	returnData.normalTextureIndex = TextureManager::GetInstance()->GetTexture(data.normalTextureName).GetSrvIndex();

	return returnData;
}