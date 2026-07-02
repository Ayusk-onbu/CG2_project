#include "Cylinder.h"
#include "ModelManager.h"

std::vector<VertexData> MakeObjectVertices(const CylinderData& data) {
	std::vector<VertexData> vertices;
	// 上リング（divide個） + 下リング（divide個） = 計 divide * 2 頂点に削減！
	vertices.reserve(data.divide * 2);
	float radianPerDivide = (2.0f * 3.141592f) / static_cast<float>(data.divide);

	// 1. まず上リングの頂点を生成
	for (uint32_t index = 0; index < data.divide; ++index) {
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float u = static_cast<float>(index) / static_cast<float>(data.divide);

		// 上側頂点 (Y = height)
		vertices.push_back({
			{ data.topRadius * -sin, data.height, data.topRadius * cos, 1.0f },
			{ u, 0.0f },
			{ -sin, 0.0f, cos }
			});
	}

	// 2. 次に下リングの頂点を生成
	for (uint32_t index = 0; index < data.divide; ++index) {
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float u = static_cast<float>(index) / static_cast<float>(data.divide);

		// 下側頂点 (Y = 0.0f)
		vertices.push_back({
			{ data.bottomRadius * -sin, 0.0f, data.bottomRadius * cos, 1.0f },
			{ u, 1.0f },
			{ -sin, 0.0f, cos }
			});
	}

	return vertices;
}

std::vector<uint32_t> MakeObjectIndices(const CylinderData& data) {
	std::vector<uint32_t> indices;
	indices.reserve(data.divide * 6); // インデックス数は三角形2つ分 (面数×6)

	for (uint32_t index = 0; index < data.divide; ++index) {
		// 現在の列の頂点インデックス
		uint32_t topCurrent = index;
		uint32_t bottomCurrent = index + data.divide;

		// 次の列の頂点インデックス（一周して戻るためのラッピング処理）
		uint32_t nextIndex = (index + 1) % data.divide;
		uint32_t topNext = nextIndex;
		uint32_t bottomNext = nextIndex + data.divide;

		// 三角形1 (左上 -> 右上 -> 左下)
		indices.push_back(topCurrent);
		indices.push_back(topNext);
		indices.push_back(bottomCurrent);

		// 三角形2 (左下 -> 右上 -> 右下)
		indices.push_back(bottomCurrent);
		indices.push_back(topNext);
		indices.push_back(bottomNext);
	}
	return indices;
}

void PrimitiveCylinder::Initialize(Fngine* engine, uint32_t numInstance) {
	PrimitiveBaseModel::Initialize(engine, numInstance);

	useModelName_ = "Cylinder";

	SetCommand = [this]() {
		auto commandList = p_engine_->GetCommand().GetList().GetList();
		auto object = ModelManager::GetInstance()->LoadObjectData(useModelName_);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Cylinder").GetRootSignature().GetRS().Get());
		commandList->SetPipelineState(PSOManager::GetInstance()->GetPSO("Cylinder").GetGPS().Get());
		commandList->IASetVertexBuffers(0, 1, &object.GetVertexBufferView());

		commandList->IASetIndexBuffer(&object.GetIndexBufferView());

		commandList->SetGraphicsRootDescriptorTable(0, instancingBuffer_->GetSRVHandleGPU());

		commandList->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetTexture("gradationLine").GetHandleGPU());

		commandList->DrawIndexedInstanced(object.GetIndexCount(), UINT(instanceDataList_.size()), 0, 0, 0);
	};
}

PrimitiveCylinderForGPU PrimitiveCylinder::ConvertToGPUData(const PrimitiveCylinderData& data) {
	PrimitiveCylinderForGPU returnData{};
	returnData.WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(data.worldTransform.mat_);
	returnData.World = data.worldTransform.mat_;
	returnData.uvTransform = data.uvTransform.mat_;
	returnData.color = data.color;
	return returnData;
}