#include "ObjectBase.h"


void ObjectBase::DrawBase() {
	//RootSignalの設定
	fngine_->GetCommand().GetList().GetList()->SetGraphicsRootSignature(fngine_->GetPSO().GetRootSignature().GetRS().Get());
	fngine_->GetCommand().GetList().GetList()->SetPipelineState(fngine_->GetPSO().GetGPS().Get());
	fngine_->GetCommand().GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//マテリアルCBufferの場所を設定
	fngine_->GetCommand().GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	fngine_->GetCommand().GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	fngine_->GetCommand().GetList().GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTexture(textureHandle_).GetHandleGPU());
	//DirectionLight用のCBufferの場所を設定
	fngine_->GetCommand().GetList().GetList()->SetGraphicsRootConstantBufferView(3, fngine_->GetLight().GetResource()->GetGPUVirtualAddress());
}

void ObjectBase::DrawBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//RootSignalの設定
	command.GetList().GetList()->SetGraphicsRootSignature(pso.GetRootSignature().GetRS().Get());
	command.GetList().GetList()->SetPipelineState(pso.GetGPS().Get());
	command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//マテリアルCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, tex.GetHandleGPU());
	//DirectionLight用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(3, light.GetResource()->GetGPUVirtualAddress());
}

void ObjectBase::DrawBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex) {
	//RootSignalの設定
	command.GetList().GetList()->SetGraphicsRootSignature(pso.GetRootSignature().GetRS().Get());
	command.GetList().GetList()->SetPipelineState(pso.GetGPS().Get());
	command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_); 
	//マテリアルCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, tex);
	//DirectionLight用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(3, light.GetResource()->GetGPUVirtualAddress());
}

void ObjectBase::DrawIndexBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	//RootSignalの設定
	command.GetList().GetList()->SetGraphicsRootSignature(pso.GetRootSignature().GetRS().Get());
	command.GetList().GetList()->SetPipelineState(pso.GetGPS().Get());
	command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// IndexBufferView(IBV)の設定
	command.GetList().GetList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, tex.GetHandleGPU());
	//DirectionLight用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(3, light.GetResource()->GetGPUVirtualAddress());
}

void ObjectBase::DrawIndexBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex) {
	//RootSignalの設定
	command.GetList().GetList()->SetGraphicsRootSignature(pso.GetRootSignature().GetRS().Get());
	command.GetList().GetList()->SetPipelineState(pso.GetGPS().Get());
	command.GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// IndexBufferView(IBV)の設定
	command.GetList().GetList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	command.GetList().GetList()->SetGraphicsRootDescriptorTable(2, tex);
	//DirectionLight用のCBufferの場所を設定
	command.GetList().GetList()->SetGraphicsRootConstantBufferView(3, light.GetResource()->GetGPUVirtualAddress());
}

void ObjectBase::InitializeMD(Vector4 color, bool isLight) {
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = color;
	materialData_->enableLighting = isLight;
	materialData_->uvTransform = Matrix4x4::Make::Identity();
}

void ObjectBase::InitializeWVPD() {
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでいく
	wvpData_->WVP = Matrix4x4::Make::Identity();
	wvpData_->World = Matrix4x4::Make::Identity();
}

void ObjectBase::SetColor(const Vector4& color) {
	materialData_->color = color;
}