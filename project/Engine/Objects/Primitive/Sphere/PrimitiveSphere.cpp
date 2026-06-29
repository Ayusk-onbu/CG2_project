#include "PrimitiveSphere.h"
#include "ModelManager.h"

void PrimitiveSphere::Initialize(Fngine* engine, uint32_t numInstance) {
	PrimitiveBaseModel::Initialize(engine, numInstance);

	useModelName_ = "Sphere";

	SetCommand = [this]() {
		auto commandList = p_engine_->GetCommand().GetList().GetList();
		auto object = ModelManager::GetInstance()->LoadObjectData(useModelName_);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Sphere").GetRootSignature().GetRS().Get());
		commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Sphere").GetGPS().Get());
		commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());

		commandList->IASetIndexBuffer(&object.GetIndexBufferView());

		commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());

		commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetTexture("GridLine").GetHandleGPU());

		commandList->DrawIndexedInstanced(object.GetIndexCount(), UINT(instanceDataList_.size()), 0, 0, 0);
	};
}

PrimitiveSphereForGPU PrimitiveSphere::ConvertToGPUData(const PrimitiveSphereData& data) {
	PrimitiveSphereForGPU returnData{};
	returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldTransform.mat_);
	returnData.World = data.worldTransform.mat_;
	returnData.color = data.color;
	return returnData;
}