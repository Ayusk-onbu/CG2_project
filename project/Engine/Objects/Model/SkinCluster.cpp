#include "ModelData.h"

void SkinCluster::Create(Fngine* engine, const Skeleton& skeleton, const ModelData& modelData) {
	// palette用のResourceを確保
	paletteResource_ = CreateBufferResource(engine->GetD3D12System().GetDevice().Get(), sizeof(WellForGPU) * skeleton.joints_.size());
	WellForGPU* mappedPalette = nullptr;
	paletteResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	// [ spanを使ってアクセス出来るように ]
	mappedPalette_ = { mappedPalette, skeleton.joints_.size() };
	paletteSrvHandle_.first = engine->GetSRV().GetCPUDescriptorHandle();
	paletteSrvHandle_.second = engine->GetSRV().GetGPUDescriptorHandle();
	// palette用のsrvを確保
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints_.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	engine->GetD3D12System().GetDevice()->CreateShaderResourceView(paletteResource_.Get(), &paletteSrvDesc, paletteSrvHandle_.first);

	// influence用のResourceを確保
	influenceResource_ = CreateBufferResource(engine->GetD3D12System().GetDevice().Get(), sizeof(VertexInfluence) * modelData.vertices.size() );
	VertexInfluence* mappedInfluence = nullptr;
	influenceResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	mappedInfluence_ = { mappedInfluence,modelData.vertices.size() };
	// influence用のVBVを確保
	influenceBufferView_.BufferLocation = influenceResource_->GetGPUVirtualAddress();
	influenceBufferView_.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	influenceBufferView_.StrideInBytes = sizeof(VertexInfluence);
	// inverseBindPoseMatrixの保存領域を作成
	inverseBindPoseMatrices_.resize(skeleton.joints_.size());
	std::generate(inverseBindPoseMatrices_.begin(), inverseBindPoseMatrices_.end(), []() {return Matrix4x4::Make::Identity();});
	// ModelDataのSkinCluster情報を解析してInfluenceの中身を埋める
	for (const auto& jointWeight : modelData.skinClusterData) {
		// Jointの名前を取得
		auto it = skeleton.jointMap_.find(jointWeight.first);
		// そんな名前は存在するのかをチェックする
		if (it == skeleton.jointMap_.end()) {
			// 無かったら次の要素へ移行
			continue;
		}
		// 該当のIndexのinverseBindPoseMatrixを取得
		inverseBindPoseMatrices_[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			// 該当のInfluenceの情報を取得
			auto& currentInfluence = mappedInfluence_[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
				// Weightがあいているかを確認する
				if (currentInfluence.weights[index] == 0.0f) {
					// Weightが0.0fなら代入する
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}
}

void SkinCluster::Update(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints_.size(); ++jointIndex) {
		assert(jointIndex < inverseBindPoseMatrices_.size());
		mappedPalette_[jointIndex].skeletonSpaceMatrix =
			inverseBindPoseMatrices_[jointIndex] * skeleton.joints_[jointIndex].skeletonSpaceMatrix;
		mappedPalette_[jointIndex].skeletonSpaceInverseTransMatrix =
			Matrix4x4::Transpose(Matrix4x4::Inverse(mappedPalette_[jointIndex].skeletonSpaceMatrix));
	}
}