#include "SpriteObject.h"

void SpriteObject::Initialize(D3D12System& d3d12, float width, float height) {
	InitializeResource(d3d12);
	InitializeData();
	InitializeVertex(width,height);
}

void SpriteObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	object_.DrawIndexBase(command, pso, light, tex);
	command.GetList().GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteObject::Draw2(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	object_.DrawIndexBase(command, pso, light, tex);
	command.GetList().GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteObject::SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv) {
	object_.wvpData_->WVP = WVP;
	object_.wvpData_->World = world;
	object_.materialData_->uvTransform = uv;
}

void SpriteObject::InitializeResource(D3D12System& d3d12) {

	object_.vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * 4);
	object_.indexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(uint32_t) * 6);
	object_.materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	object_.wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));

}

void SpriteObject::InitializeData() {
	object_.InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), false);
	object_.InitializeWVPD();
	//リソースの先頭のアドレスから使う
	object_.vertexBufferView_.BufferLocation = object_.vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	object_.vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;//
	//１個当たりのサイズ
	object_.vertexBufferView_.StrideInBytes = sizeof(VertexData);//
	// リソースの先頭のアドレスから使う
	object_.indexBufferView_.BufferLocation = object_.indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス６つ分のサイズ
	object_.indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	object_.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	object_.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&object_.vertexData_));
}

void SpriteObject::InitializeVertex(float width, float height) {
	//1枚目の三角形
	object_.vertexData_[0].position = { 0.0f,height,0.0f,1.0f };
	object_.vertexData_[0].texcoord = { 0.0f,1.0f };
	object_.vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	object_.vertexData_[1].texcoord = { 0.0f,0.0f };
	object_.vertexData_[2].position = { width,height,0.0f,1.0f };
	object_.vertexData_[2].texcoord = { 1.0f,1.0f };
	object_.vertexData_[3].position = { width, 0.0f, 0.0f, 1.0f };
	object_.vertexData_[3].texcoord = { 1.0f, 0.0f };
	/*object_.vertexData_[0].position = { width/-2.0f,height/2.0f,0.0f,1.0f };
	object_.vertexData_[0].texcoord = { 0.0f,1.0f };
	object_.vertexData_[1].position = { width / -2.0f,height / -2.0f,0.0f,1.0f };
	object_.vertexData_[1].texcoord = { 0.0f,0.0f };
	object_.vertexData_[2].position = { width / 2.0f,height / 2.0f,0.0f,1.0f };
	object_.vertexData_[2].texcoord = { 1.0f,1.0f };
	object_.vertexData_[3].position = { width / 2.0f,height / -2.0f, 0.0f, 1.0f };
	object_.vertexData_[3].texcoord = { 1.0f, 0.0f };*/

	object_.indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&object_.indexData_));
	object_.indexData_[0] = 0;object_.indexData_[1] = 1;object_.indexData_[2] = 2;
	object_.indexData_[3] = 1;object_.indexData_[4] = 3;object_.indexData_[5] = 2;
}