#include "LineObject.h"

void LineObject::Initialize(D3D12System& d3d12, float width, float height) {
	InitializeResource(d3d12);
	InitializeData();
	InitializeVertex(width, height);
}

void LineObject::Initialize(D3D12System& d3d12, const Vector3& start, const Vector3& end) {
	InitializeResource(d3d12);
	InitializeData();
	InitializeVertex(start, end);
}

void LineObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	object_.DrawBase(command, pso, light, tex);
	command.GetList().GetList()->DrawInstanced(2, 1, 0, 0);
}

void LineObject::SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv) {
	object_.wvpData_->WVP = WVP;
	object_.wvpData_->World = world;
	object_.materialData_->uvTransform = uv;
}

void LineObject::SetColor(Vector4 color) {
	object_.materialData_->color = color;
}

void LineObject::InitializeResource(D3D12System& d3d12) {

	object_.vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * 2);
	object_.materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	object_.wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));

}

void LineObject::InitializeData() {
	object_.InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), false);
	object_.InitializeWVPD();
	//リソースの先頭のアドレスから使う
	object_.vertexBufferView_.BufferLocation = object_.vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん2つ分
	object_.vertexBufferView_.SizeInBytes = sizeof(VertexData) * 2;//
	//１個当たりのサイズ
	object_.vertexBufferView_.StrideInBytes = sizeof(VertexData);//
	
	object_.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&object_.vertexData_));
}

void LineObject::InitializeVertex(float width, float height) {
	object_.vertexData_[0].position = { -width / 2.0f,-height / 2.0f,0.0f,1.0f };
	object_.vertexData_[0].texcoord = { 0.0f,0.0f };
	object_.vertexData_[1].position = { width / 2.0f,height / 2.0f,0.0f,1.0f };
	object_.vertexData_[1].texcoord = { 1.0f,1.0f };
}

void LineObject::InitializeVertex(const Vector3& start, const Vector3& end) {
	object_.vertexData_[0].position = { start.x,start.y,start.z,1.0f };
	object_.vertexData_[0].texcoord = { 0.0f,0.0f };
	object_.vertexData_[1].position = { end.x,end.y,end.z,1.0f };
	object_.vertexData_[1].texcoord = { 1.0f,1.0f };
}