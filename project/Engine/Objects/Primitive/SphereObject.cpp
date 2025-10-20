#include "SphereObject.h"

void SphereObject::Initialize(D3D12System& d3d12) {
	kSubdivision_ = 16.0f;
	vertexCount = static_cast<uint32_t>(kSubdivision_ * kSubdivision_) * 6;
	InitializeResource(d3d12);
	InitializeData();
	InitializeVertex();
}

void SphereObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	object_.DrawBase(command, pso, light, tex);
	command.GetList().GetList()->DrawInstanced(vertexCount, 1, 0, 0);
}

void SphereObject::SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv) {
	object_.wvpData_->WVP = WVP;
	object_.wvpData_->World = world;
	object_.materialData_->uvTransform = uv;
}

void SphereObject::InitializeResource(D3D12System& d3d12) {

	object_.vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * vertexCount);
	//object_.indexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(uint32_t) * 3);
	object_.materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	object_.wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));

}

void SphereObject::InitializeData() {
	object_.InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), true);
	object_.InitializeWVPD();
	//リソースの先頭のアドレスから使う
	object_.vertexBufferView_.BufferLocation = object_.vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	object_.vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount;//
	//１個当たりのサイズ
	object_.vertexBufferView_.StrideInBytes = sizeof(VertexData);//
	// リソースの先頭のアドレスから使う
	//object_.indexBufferView_.BufferLocation = object_.indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス６つ分のサイズ
	//object_.indexBufferView_.SizeInBytes = sizeof(uint32_t) * 3;
	// インデックスはuint32_tとする
	//object_.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	object_.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&object_.vertexData_));
}

void SphereObject::InitializeVertex() {
	uint32_t latIndex = 0;
	uint32_t lonIndex = 0;
	//経度分割１つ分の角度φd
	const float kLonEvery = float(3.14159265358979323846f) * 2.0f / static_cast<float>(kSubdivision_);
	//緯度分割1つ分の角度θd
	const float kLatEvery = float(3.14159265358979323846f) / static_cast<float>(kSubdivision_);
	//緯度の方向に分割
	for (latIndex = 0;latIndex < kSubdivision_; ++latIndex) {
		float lat = -float(3.14159265358979323846f) / 2.0f + kLatEvery * latIndex;//θ
		//緯度の方向に分割しながら線を描く
		for (lonIndex = 0;lonIndex < kSubdivision_;++lonIndex) {
			uint32_t start = static_cast<uint32_t>(latIndex * kSubdivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//φ
			//頂点にデータを入力する。基準点a
			object_.vertexData_[start + 0].position.x = std::cos(lat) * std::cos(lon);
			object_.vertexData_[start + 0].position.y = std::sin(lat);
			object_.vertexData_[start + 0].position.z = std::cos(lat) * std::sin(lon);
			object_.vertexData_[start + 0].position.w = 1.0f;
			object_.vertexData_[start + 0].texcoord.x = lonIndex / kSubdivision_;
			object_.vertexData_[start + 0].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision_);
			//基準点b
			object_.vertexData_[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			object_.vertexData_[start + 1].position.y = std::sin(lat + kLatEvery);
			object_.vertexData_[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			object_.vertexData_[start + 1].position.w = 1.0f;
			object_.vertexData_[start + 1].texcoord.x = lonIndex / kSubdivision_;
			object_.vertexData_[start + 1].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision_);
			//基準点c
			object_.vertexData_[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			object_.vertexData_[start + 2].position.y = std::sin(lat);
			object_.vertexData_[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			object_.vertexData_[start + 2].position.w = 1.0f;
			object_.vertexData_[start + 2].texcoord.x = (lonIndex + 1) / kSubdivision_;
			object_.vertexData_[start + 2].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision_);
			//基準点d
			object_.vertexData_[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			object_.vertexData_[start + 3].position.y = std::sin(lat + kLatEvery);
			object_.vertexData_[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			object_.vertexData_[start + 3].position.w = 1.0f;
			object_.vertexData_[start + 3].texcoord.x = (lonIndex + 1) / kSubdivision_;
			object_.vertexData_[start + 3].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision_);
			//基準点e
			object_.vertexData_[start + 4].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			object_.vertexData_[start + 4].position.y = std::sin(lat);
			object_.vertexData_[start + 4].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			object_.vertexData_[start + 4].position.w = 1.0f;
			object_.vertexData_[start + 4].texcoord.x = (lonIndex + 1) / kSubdivision_;
			object_.vertexData_[start + 4].texcoord.y = 1.0f - float(latIndex) / float(kSubdivision_);
			//基準点f
			object_.vertexData_[start + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			object_.vertexData_[start + 5].position.y = std::sin(lat + kLatEvery);
			object_.vertexData_[start + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			object_.vertexData_[start + 5].position.w = 1.0f;
			object_.vertexData_[start + 5].texcoord.x = lonIndex / kSubdivision_;
			object_.vertexData_[start + 5].texcoord.y = 1.0f - float(latIndex + 1) / float(kSubdivision_);
	
			for (int i = 0;i < 6;i++) {
				object_.vertexData_[start + i].normal.x = object_.vertexData_[start + i].position.x;
				object_.vertexData_[start + i].normal.y = object_.vertexData_[start + i].position.y;
				object_.vertexData_[start + i].normal.z = object_.vertexData_[start + i].position.z;
			}
		}
	}
}