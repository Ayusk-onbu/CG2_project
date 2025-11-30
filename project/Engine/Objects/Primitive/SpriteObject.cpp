#include "SpriteObject.h"
#include "Fngine.h"

// ----------------------------
// Public Function
// ----------------------------

void SpriteObject::Initialize(int textureHandle,SPRITE_ANCHOR_TYPE type) {
	textureHandle_ = textureHandle;
	anchorType_ = type;
	textureSize_ = TextureManager::GetInstance()->GetTexture(textureHandle_).GetSize();
	InitializeResource(fngine_->GetD3D12System());
	InitializeData();
	InitializeVertex();
	worldTransform_.Initialize();
	uvTransform_.Initialize();
}

void SpriteObject::Draw(SPRITE_VIEW_TYPE type) {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
	switch (type) {
	case SPRITE_VIEW_TYPE::UI:
		SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawUI(worldTransform_.mat_));
		break;
	case SPRITE_VIEW_TYPE::Object:
		SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(worldTransform_.mat_));
		break;
	}
	Draw(fngine_->GetCommand(),
		PSOManager::GetInstance()->GetPSO("SpriteObject3D"),
		fngine_->GetLight(),
		TextureManager::GetInstance()->GetTexture(textureHandle_));
}

void SpriteObject::SetTextureSize(const Vector2& leftTop, const Vector2& size) {
	float tex_left = leftTop.x / textureSize_.x;
	float tex_right = (leftTop.x + size.x) / textureSize_.x;
	float tex_top = leftTop.y / textureSize_.y;
	float tex_bottom = (leftTop.y + size.y) / textureSize_.y;

	// 左下
	vertexData_[0].texcoord = { tex_left,tex_bottom };
	// 左上
	vertexData_[1].texcoord = { tex_left,tex_top };
	// 右下
	vertexData_[2].texcoord = { tex_right,tex_bottom };
	// 右上
	vertexData_[3].texcoord = { tex_right, tex_top };
}

// ------------------------------
// Private Function
// ------------------------------

void SpriteObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawIndexBase(command, pso, light, tex);
	command.GetList().GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteObject::Draw2(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawIndexBase(command, pso, light, tex);
	command.GetList().GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteObject::SetWVPData(Matrix4x4 WVP) {
	wvpData_->WVP = WVP;
	wvpData_->World = worldTransform_.mat_;
	wvpData_->worldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(worldTransform_.mat_));
	materialData_->uvTransform = uvTransform_.mat_;
}

void SpriteObject::InitializeResource(D3D12System& d3d12) {
	vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * 4);
	indexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(uint32_t) * 6);
	materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));
}

void SpriteObject::InitializeData() {
	InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), false);
	InitializeWVPD();

	// ----------------------
	//  Veretex Buffer View
	// ----------------------

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;//
	//１個当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);//
	
	// ----------------------
	// Index Buffer View
	// ----------------------

	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	
}

void SpriteObject::InitializeVertex() {
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	switch (anchorType_) {
	case SPRITE_ANCHOR_TYPE::LeftTop:
		anchorPoint_ = { 0.0f,0.0f };
		break;
	case SPRITE_ANCHOR_TYPE::LeftBottom:
		anchorPoint_ = { 0.0f,1.0f };
		break;
	case SPRITE_ANCHOR_TYPE::LeftMiddle:
		anchorPoint_ = { 0.0f,0.5f };
		break;
	case SPRITE_ANCHOR_TYPE::RightTop:
		anchorPoint_ = { 1.0f,0.0f };
		break;
	case SPRITE_ANCHOR_TYPE::RightBottom:
		anchorPoint_ = { 1.0f,1.0f };
		break;
	case SPRITE_ANCHOR_TYPE::RightMiddle:
		anchorPoint_ = { 0.0f,0.5f };
		break;
	case SPRITE_ANCHOR_TYPE::Middle:
		anchorPoint_ = { 0.5f,0.5f };
		break;
	}

	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}
	float sizeX = textureSize_.x;
	float sizeY = textureSize_.y;
	// 左下
	vertexData_[0].position = { left * sizeX,bottom * sizeY,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	// 左上
	vertexData_[1].position = { left * sizeX,top * sizeY,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	// 右下
	vertexData_[2].position = { right * sizeX,bottom * sizeY,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	// 右上
	vertexData_[3].position = { right * sizeX, top * sizeY, 0.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;indexData_[1] = 1;indexData_[2] = 2;
	indexData_[3] = 1;indexData_[4] = 3;indexData_[5] = 2;
}