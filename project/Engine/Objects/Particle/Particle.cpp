#include "Particle.h"
#include "CameraSystem.h"
#include "TextureManager.h"
#include "Easing.h"

Particle::Particle(Fngine* fngine)
	: p_fngine_(fngine){}

void Particle::Initialize(uint32_t numInstance) {

	// インスタンス数
	numMaxInstance_ = numInstance;

	textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/circle.png");

	vertexResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(VertexData) * 4);
	indexResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(uint32_t) * 6);
	materialResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(Material));
	
	// Resourceを作成
	instancingResource_ = CreateBufferResource(
		p_fngine_->GetD3D12System().GetDevice().Get(),
		sizeof(ParticleForGPU) * numMaxInstance_);

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = {1.0f,1.0f,1.0f,1.0f};
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix4x4::Make::Identity();
	materialData_->shininess = 0.0f;

	// アドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	// ParticleForGPUを初期化
	for (uint32_t index = 0;index < numMaxInstance_;index++) {
		instancingData_[index].WVP = Matrix4x4::Make::Identity();
		instancingData_[index].World = Matrix4x4::Make::Identity();
		instancingData_[index].color = Vector4(1.0f,1.0f,1.0f,1.0f);
	}

	// -------------------------------
	// SRVの作成
	// -------------------------------

	instancingDesc_.Format = DXGI_FORMAT_UNKNOWN;
	instancingDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingDesc_.Buffer.FirstElement = 0;
	instancingDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingDesc_.Buffer.NumElements = numMaxInstance_;
	instancingDesc_.Buffer.StructureByteStride = sizeof(ParticleForGPU);

	instancingSrvHandleCPU_ = p_fngine_->GetSRV().GetCPUDescriptorHandle();
	instancingSrvHandleGPU_ = p_fngine_->GetSRV().GetGPUDescriptorHandle();

	p_fngine_->GetD3D12System().GetDevice()->CreateShaderResourceView(
		instancingResource_.Get(), &instancingDesc_, instancingSrvHandleCPU_
	);

	for (uint32_t i = 0;i < numMaxInstance_;++i) {
		Create();
	}

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはちゅてん三つ分
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;//
	//１個当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);//
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//1枚目の三角形
	vertexData_[0].position = { 0.0f,height_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { width_,height_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { width_, 0.0f, 0.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;indexData_[1] = 1;indexData_[2] = 2;
	indexData_[3] = 1;indexData_[4] = 3;indexData_[5] = 2;
}

void Particle::Update() {

	const float kDeltaTime_ = 1.0f / 60.0f;

	auto it = info_.begin();
	while (it != info_.end()) {
		Info* info = it->get();

		// 1. Bulletの更新（移動・寿命チェック）
		info->worldTransform.set_.Translation(
			info->worldTransform.get_.Translation() + (info->velocity * 10.0f) * kDeltaTime_);

		info->worldTransform.LookAtToVector(CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation());

		float alpha = Easing_Float(1.0f, 0.0f, info->currentTime, info->lifeTime, EASINGTYPE::None);
		info->color.w = alpha;

		// 2. 死亡判定
		if (info->lifeTime <= info->currentTime) {

			// 死亡したBulletをリストから削除
			// unique_ptr なので、リストから削除されると自動でデストラクタが呼ばれメモリ解放される
			it = info_.erase(it);
		}
		else {
			// 削除されなかった場合はイテレータを進める
			++it;
		}
	}

	if (InputManager::IsJump()) {
		Create();
	}
}

void Particle::Draw() {
	uint32_t numInstance = 0;
	uint32_t index = 0;
	for (auto& info : info_) {
		info->currentTime += 1.0f / 60.0f;
		info->worldTransform.LocalToWorld();
		instancingData_[index].WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(info->worldTransform.mat_);
		instancingData_[index].World = info->worldTransform.mat_;
		instancingData_[index].color = info->color;
		index++;
		numInstance++;
	}
	p_fngine_->GetCommand().GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	p_fngine_->GetCommand().GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//RootSignalの設定
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSO("Structured").GetRootSignature().GetRS().Get());
	p_fngine_->GetCommand().GetList().GetList()->SetPipelineState(PSOManager::GetInstance()->GetPSO("Structured").GetGPS().Get());
	p_fngine_->GetCommand().GetList().GetList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// IndexBufferView(IBV)の設定
	p_fngine_->GetCommand().GetList().GetList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTexture(textureHandle_).GetHandleGPU());

	p_fngine_->GetCommand().GetList().GetList()->DrawIndexedInstanced(6, numInstance, 0, 0, 0);
}

void Particle::Create() {
	if (info_.size() < numMaxInstance_) {
		std::unique_ptr<Info> info = std::make_unique<Info>();
		info->worldTransform.Initialize();
		info->worldTransform.set_.Scale({ 1.0f,1.0f,1.0f });
		info->worldTransform.set_.Rotation({ 0.0f,0.0f,0.0f });
		info->worldTransform.set_.Translation({ RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f) + 300.0f,
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f) + 300.0f ,
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f) }
		);
		info->worldTransform.LocalToWorld();
		info->velocity = { RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f),
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f),
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(-1.0f,1.0f)
		};
		info->color = {
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(0.0f,1.0f),
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(0.0f,1.0f),
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(0.0f,1.0f),
			RandomUtils::GetInstance()->GetHighRandom().GetFloat(0.0f,1.0f)
		};
		info->lifeTime = RandomUtils::GetInstance()->GetHighRandom().GetFloat(10.0f, 50.0f);
		info->currentTime = 0.0f;
		info_.push_back(std::move(info));
	}
}