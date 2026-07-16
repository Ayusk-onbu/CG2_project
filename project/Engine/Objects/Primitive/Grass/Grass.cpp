#include "Grass.h"
#include "ModelManager.h"
#include "Chronos.h"

void Grass::Initialize(Fngine* engine, uint32_t numInstance){
	PrimitiveBaseModel::Initialize(engine, numInstance);
	useModelName_ = "Grass"; // 草の3Dモデル（シンプルな板状など）

	SetCommand = [this]() {
		auto commandList = p_engine_->GetCommand().GetList().GetList();
		auto object = ModelManager::GetInstance()->LoadObjectData(useModelName_);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Grass").GetRootSignature().GetRS().Get());
		commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Grass").GetGPS().Get());
		commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());
		commandList->IASetIndexBuffer(&object.GetIndexBufferView());

		commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());
		commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetTexture("GridLine").GetHandleGPU());

		// シェーダー（HLSL）側で風のうねりをSin波で計算
		float time = static_cast<float>(Chronos::GetInstance()->GetTotalTime());
		commandList->SetGraphicsRoot32BitConstant(2, *(UINT*)&time, 0);

		commandList->DrawIndexedInstanced(object.GetIndexCount(), UINT(instanceDataList_.size()), 0, 0, 0);
	};
}

GrassForGPU Grass::ConvertToGPUData(const GrassObjectData& data){
	GrassForGPU returnData{};
	returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldTransform.mat_);
	returnData.World = data.worldTransform.mat_;
	returnData.color = data.color;
	returnData.windPhase = data.windPhase; // シェーダーに送る
	return returnData;
}