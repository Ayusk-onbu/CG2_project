#include "Line.h"
#include "ModelManager.h"

std::vector<VertexData> MakeLineVertices() {
	std::vector<VertexData> vertices;
	// 0: 始点 (原点)
	vertices.push_back({ { 0.0f, 0.0f, 0.0f, 1.0f }, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
	// 1: 終点 (Z方向に1)
	vertices.push_back({ { 0.0f, 0.0f, 1.0f, 1.0f }, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
	return vertices;
}

// インデックスも単純に 0番 と 1番 を結ぶだけ
std::vector<uint32_t> MakeLineIndices() {
	return { 0, 1 };
}

void PrimitiveLine::Initialize(Fngine* engine, uint32_t numInstance) {
	PrimitiveBaseModel::Initialize(engine, numInstance);

	useModelName_ = "Line";

	SetCommand = [this]() {
		auto commandList = p_engine_->GetCommand().GetList().GetList();
		
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Line").GetRootSignature().GetRS().Get());
		commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Line").GetGPS().Get());
		
		commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());

		commandList->DrawInstanced(2, UINT(instanceDataList_.size()), 0, 0);
		};
}

PrimitiveLineForGPU PrimitiveLine::ConvertToGPUData(const PrimitiveLineData& data) {
	auto camera = CameraSystem::GetInstance()->GetActiveCamera();
	PrimitiveLineForGPU returnData{};
	returnData.startPoint = Matrix4x4::Transform(camera->GetViewProjectionMatrix(), {data.startPoint.x, data.startPoint.y, data.startPoint.z, 1.0f});
	returnData.endPoint = Matrix4x4::Transform(camera->GetViewProjectionMatrix(), { data.endPoint.x, data.endPoint.y, data.endPoint.z, 1.0f });
	returnData.color = data.color;
	return returnData;
}