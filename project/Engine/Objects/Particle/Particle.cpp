#include "Particle.h"
#include "CameraSystem.h"
#include "TextureManager.h"
#include "Easing.h"

using json = nlohmann::json;

Particle::Particle(Fngine* fngine)
	: p_fngine_(fngine){

}

void Particle::Initialize(uint32_t numInstance) {

	// パラメータの設定
	// インスタンス数
	numMaxInstance_ = numInstance;
	// [ 使うTextureの設定 ]
	// 固定なのキモすぎ
	textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/circle.png");

	vertexResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(VertexData) * 4);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	indexResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(uint32_t) * 6);
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	materialResource_ = CreateBufferResource(p_fngine_->GetD3D12System().GetDevice().Get(), sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = Matrix4x4::Make::Identity();
	materialData_->shininess = 0.0f;

	instancingBuffer_ = std::make_unique<Structured<ParticleForGPU>>(p_fngine_);
	instancingBuffer_->Initialize(numMaxInstance_);
	ParticleForGPU* instancingData = instancingBuffer_->GetMappedData();

	for (uint32_t index = 0;index < numMaxInstance_;index++) {
		instancingData[index].WVP = Matrix4x4::Make::Identity();
		instancingData[index].World = Matrix4x4::Make::Identity();
		instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	// -----------------------------
	// Emitter
	// -----------------------------

	std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
	newEmitter->Initialize(p_fngine_);
	newEmitter->worldTransform_.Initialize();
	newEmitter->worldTransform_.set_.Translation({ 0.0f,10.0f,0.0f });
	newEmitter->emitNum_ = 1;
	newEmitter->spawnRadius_ = 0.5f;
	newEmitter->minVelocity_ = { 0.0f,75.0f,0.0f };
	newEmitter->maxVelocity_ = { 0.0f,75.0f,0.0f };
	newEmitter->startColor_ = { 0.2f,0.5f,1.0f,1.0f };
	newEmitter->SetMinLifeTime(6.0f);
	newEmitter->SetMaxLifeTime(6.0f);
	newEmitter->name_ = "Middle";
	emitters_.push_back(std::move(newEmitter));

	MakeEmitter();
	ParticleEmitter* emitter = emitters_.front().get();
	/*for (uint32_t i = 0;i < numMaxInstance_;++i) {
		emitter->Emit(this);
	}*/

	// -----------------------------
	// Force Field
	// -----------------------------

	MakeForceField();

	// [ 重力場 ]
	/*std::unique_ptr<GravityForceField>gravity = std::make_unique<GravityForceField>(Vector3{ 0.0f,-9.8f,0.0f });
	gravity->Initialize(p_fngine_);
	gravity->radius_ = 5.0f;
	gravity->name_ = "First";
	forceFields_.push_back(std::move(gravity));*/

	// [ 吸引 ] 
	/*std::unique_ptr<PointForceField>attractor = std::make_unique<PointForceField>(20.0f);
	attractor->Initialize(p_fngine_);
	attractor->worldTransform_.Initialize();
	attractor->worldTransform_.set_.Translation({ 0.0f,10.0f,0.0f });
	attractor->radius_ = 50.0f;
	forceFields_.push_back(std::move(attractor));*/

	// [ 斥力 ]
	/*std::unique_ptr<PointForceField>repulsor = std::make_unique<PointForceField>(-0.05f);
	repulsor->Initialize(p_fngine_);
	repulsor->worldTransform_.Initialize();
	repulsor->worldTransform_.set_.Translation({ -2.0f,1.5f,0.0f });
	repulsor->radius_ = 1.0f;
	forceFields_.push_back(std::move(repulsor));*/

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

	//1枚目の三角形
	vertexData_[0].position = { 0.0f,height_,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { width_,height_,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { width_, 0.0f, 0.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };
	
	indexData_[0] = 0;indexData_[1] = 1;indexData_[2] = 2;
	indexData_[3] = 1;indexData_[4] = 3;indexData_[5] = 2;

}

void Particle::Update() {

	const float kDeltaTime_ = 1.0f / 60.0f;

	for (auto& emitter : emitters_) {
		emitter->Update(this);
	}

	auto it = info_.begin();
	while (it != info_.end()) {
		ParticleData* info = it->get();

		// ----------------------------
		// Force Fieldの適用
		// ----------------------------
		for (auto& field : forceFields_) {
			field->ApplyForce(info);
		}

		// 1. Bulletの更新（移動・寿命チェック）
		info->worldTransform.set_.Translation(
			info->worldTransform.get_.Translation() + (info->velocity) * kDeltaTime_);

		info->worldTransform.LookAtToVector(CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation());

		float currentScale = Easing_Float(info->startScale, info->endScale, info->currentTime, info->lifeTime, EASINGTYPE::None);
		info->worldTransform.set_.Scale({ currentScale,currentScale,currentScale });

		info->color = Easing_Vector4(info->startColor, info->endColor, info->currentTime, info->lifeTime, EASINGTYPE::None);

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
	int size = static_cast<int>(info_.size());
	ImGui::Begin("Particle");
	ImGui::SliderInt("Num",&size,0,0);
	// 1. 名前の入力フィールド
	// std::stringのポインタを渡すオーバーロードを使用
	char buf[128];
	strncpy_s(buf, sizeof(buf), input_name_buffer.c_str(), _TRUNCATE);
	buf[sizeof(buf) - 1] = 0; // 確実にNULL終端

	if (ImGui::InputText("Emitter Name", buf, sizeof(buf))) {
		// 入力が変更されたらstd::stringを更新
		input_name_buffer = buf;
	}
	if (ImGui::Button("Gravity")) {
		AddGravityForce(input_name_buffer);
	}
	if (ImGui::Button("Point")) {
		AddPointForce(input_name_buffer);
	}
	if (ImGui::Button("Emitter")) {
		AddParticleEmitter(input_name_buffer);
	}
	ImGui::End();
}

void Particle::Draw() {
	uint32_t numInstance = 0;
	uint32_t index = 0;

	ParticleForGPU* instancingData = instancingBuffer_->GetMappedData();

	for (auto& info : info_) {
		info->currentTime += 1.0f / 60.0f;
		info->worldTransform.LocalToWorld();
		instancingData[index].WVP = CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(info->worldTransform.mat_);
		instancingData[index].World = info->worldTransform.mat_;
		instancingData[index].color = info->color;
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
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootDescriptorTable(1, instancingBuffer_->GetSRVHandleGPU());
	//SRVのDescritorTableの先頭を設定。2はrootParameter[2]である
	p_fngine_->GetCommand().GetList().GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetTexture(textureHandle_).GetHandleGPU());

	p_fngine_->GetCommand().GetList().GetList()->DrawIndexedInstanced(6, numInstance, 0, 0, 0);
}

void Particle::AddParticle(std::unique_ptr<ParticleData> data)
{
	// 1. 最大インスタンス数に達していないかチェック
	if (info_.size() < numMaxInstance_) {
		// 2. 渡された unique_ptr の所有権をシステム内のリストに移動（ムーブ）
		//    std::move(data) が実行されると、引数 data が保持していたポインタの所有権が
		//    info_.push_back の新しい要素に移譲され、data は null になります。
		info_.push_back(std::move(data));
	}
	// 3. 最大数に達している場合（else:）、
	//    渡された unique_ptr (data) は、リストに追加されずに
	//    この関数のスコープを抜けるときに自動的に解放（delete）されます。
}

void Particle::DrawDebug() {
	// DrawDebugの描画
	// [ Emitter ]
	for (const auto& emitter : emitters_) {
		emitter->DrawDebug();
	}

	// [ ForceField ]
	for (const auto& filed : forceFields_) {
		filed->DrawDebug();
	}
}

void Particle::AddParticleEmitter(std::string name) {
	std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
	newEmitter->Initialize(p_fngine_);
	newEmitter->worldTransform_.Initialize();
	newEmitter->worldTransform_.set_.Translation({ 0.0f,0.0f,0.0f });
	newEmitter->emitNum_ = 3;
	newEmitter->SetMinLifeTime(2.0f);
	newEmitter->SetMaxLifeTime(10.0f);
	newEmitter->name_ = name;
	emitters_.push_back(std::move(newEmitter));
}

void Particle::AddPointForce(std::string name) {
	std::unique_ptr<PointForceField>attractor = std::make_unique<PointForceField>(20.0f);
	attractor->Initialize(p_fngine_);
	attractor->worldTransform_.Initialize();
	attractor->worldTransform_.set_.Translation({ 0.0f,10.0f,0.0f });
	attractor->radius_ = 50.0f;
	attractor->name_ = name;
	forceFields_.push_back(std::move(attractor));
}

void Particle::AddGravityForce(std::string name) {
	std::unique_ptr<GravityForceField>gravity = std::make_unique<GravityForceField>(Vector3{ 0.0f,-9.8f,0.0f });
	gravity->Initialize(p_fngine_);
	gravity->radius_ = 5.0f;
	gravity->name_ = name;
	forceFields_.push_back(std::move(gravity));
}

void Particle::MakeEmitter() {
	// 定数
	int EMITTER_COUNT = 12; // 生成するエミッターの数
	float CIRCLE_RADIUS = 10.0f; // 配置する円の半径
	float PI = 3.1415926535f;

	// Emitterのリストを保持するコンテナ（例: std::vector<std::unique_ptr<ParticleEmitter>>)
	// std::vector<std::unique_ptr<ParticleEmitter>> emitters_; 

	for (int i = 0; i < EMITTER_COUNT; ++i) {
		// 角度を計算
		float angle = (float)i / EMITTER_COUNT * 2.0f * PI;

		// 円周上の座標を計算 (X-Z平面を想定)
		float x = CIRCLE_RADIUS * std::cos(angle);
		float z = CIRCLE_RADIUS * std::sin(angle);

		// 💡 1. Emitterの生成と基本設定
		std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
		newEmitter->Initialize(p_fngine_);
		newEmitter->worldTransform_.Initialize();

		// 💡 2. Emitterの位置を円形に設定
		newEmitter->worldTransform_.set_.Translation({ x, 0.0f, z }); // Y軸は 0.0f 固定

		// 💡 3. パーティクルパラメータの設定 (既存の設定を流用)
		newEmitter->emitNum_ = 1;
		newEmitter->SetMinLifeTime(5.0f);
		newEmitter->SetMaxLifeTime(6.0f);

		// 💡 4. 初期速度の調整 (重要: パーティクルを外側または内側に飛ばす)
		// ここでは、パーティクルが円の中心に向かって飛ぶように設定
		float initialSpeed = 2.0f; // 飛び出す速さ

		// Emitterから円の中心(0, 0, 0)に向かうベクトルを計算
		// {-x, 0.0f, -z} を正規化し、initialSpeedを掛ける

		// 中心に向かう方向ベクトル
		float dir_x = -x / CIRCLE_RADIUS;
		float dir_z = -z / CIRCLE_RADIUS;

		// min/max Velocityを設定
		newEmitter->minVelocity_ = { dir_x * initialSpeed, 0.0f, dir_z * initialSpeed };
		newEmitter->maxVelocity_ = { dir_x * initialSpeed, 0.0f, dir_z * initialSpeed };

		// 💡 ランダム性を加える場合:
		// newEmitter->minVelocity_ = {dir_x * initialSpeed - 0.5f, -0.5f, dir_z * initialSpeed - 0.5f};
		// newEmitter->maxVelocity_ = {dir_x * initialSpeed + 0.5f, 0.5f, dir_z * initialSpeed + 0.5f};


		newEmitter->name_ = "Emitter_" + std::to_string(i);
		// スポーン範囲は、Emitterの位置を中心としてパーティクルを生成する範囲。
		// 小さく（例: 0.1f）設定すると、Emitterの位置から正確に発射されます。
		newEmitter->spawnRadius_ = 0.1f;
		newEmitter->startColor_ = { 0.22f,0.7f,1.0f,1.0f };
		newEmitter->endColor_ = { 0.5f,0.5f,0.0f,0.0f };

		// リストに追加
		emitters_.push_back(std::move(newEmitter));
	}

	// 定数
	EMITTER_COUNT = 24; // 生成するエミッターの数
	CIRCLE_RADIUS = 24.0f; // 配置する円の半径

	// Emitterのリストを保持するコンテナ（例: std::vector<std::unique_ptr<ParticleEmitter>>)
	// std::vector<std::unique_ptr<ParticleEmitter>> emitters_; 

	for (int i = 0; i < EMITTER_COUNT; ++i) {
		// 角度を計算
		float angle = (float)i / EMITTER_COUNT * 2.0f * PI;

		// 円周上の座標を計算 (X-Z平面を想定)
		float x = CIRCLE_RADIUS * std::cos(angle);
		float z = CIRCLE_RADIUS * std::sin(angle);

		// 💡 1. Emitterの生成と基本設定
		std::unique_ptr<ParticleEmitter> newEmitter = std::make_unique<ParticleEmitter>();
		newEmitter->Initialize(p_fngine_);
		newEmitter->worldTransform_.Initialize();

		// 💡 2. Emitterの位置を円形に設定
		newEmitter->worldTransform_.set_.Translation({ x, -5.0f, z }); // Y軸は 0.0f 固定

		// 💡 3. パーティクルパラメータの設定 (既存の設定を流用)
		newEmitter->emitNum_ = 1;
		newEmitter->SetMinLifeTime(5.0f);
		newEmitter->SetMaxLifeTime(6.0f);

		// 💡 4. 初期速度の調整 (重要: パーティクルを外側または内側に飛ばす)
		// ここでは、パーティクルが円の中心に向かって飛ぶように設定
		float initialSpeed = -2.0f; // 飛び出す速さ

		// Emitterから円の中心(0, 0, 0)に向かうベクトルを計算
		// {-x, 0.0f, -z} を正規化し、initialSpeedを掛ける

		// 中心に向かう方向ベクトル
		float dir_x = -x / CIRCLE_RADIUS;
		float dir_z = -z / CIRCLE_RADIUS;

		// min/max Velocityを設定
		newEmitter->minVelocity_ = { dir_x * initialSpeed, 8.0f, dir_z * initialSpeed };
		newEmitter->maxVelocity_ = { dir_x * initialSpeed, 8.0f, dir_z * initialSpeed };

		// 💡 ランダム性を加える場合:
		// newEmitter->minVelocity_ = {dir_x * initialSpeed - 0.5f, -0.5f, dir_z * initialSpeed - 0.5f};
		// newEmitter->maxVelocity_ = {dir_x * initialSpeed + 0.5f, 0.5f, dir_z * initialSpeed + 0.5f};


		newEmitter->name_ = "Emitter_Ver2" + std::to_string(i);
		// スポーン範囲は、Emitterの位置を中心としてパーティクルを生成する範囲。
		// 小さく（例: 0.1f）設定すると、Emitterの位置から正確に発射されます。
		newEmitter->spawnRadius_ = 0.1f;
		newEmitter->startColor_ = { 1.0f,1.0f,1.0f,1.0f };
		newEmitter->endColor_ = { 1.0f,1.0f,1.0f,0.0f };

		// リストに追加
		emitters_.push_back(std::move(newEmitter));
	}
}

void Particle::MakeForceField() {
	// 🌳 A. 全体重力 (弱く設定し、落下感を出す)
	std::unique_ptr<GravityForceField> gravity = std::make_unique<GravityForceField>(Vector3{ 0.0f, -3.0f, 0.0f });
	gravity->Initialize(p_fngine_);
	gravity->radius_ = 50.0f; // 広い範囲に影響
	gravity->name_ = "WeakGravity";
	forceFields_.push_back(std::move(gravity));

	// 🌲 B. 幹の吸引力 (パーティクルを中央に引き寄せる)
	std::unique_ptr<PointForceField> trunkAttractor = std::make_unique<PointForceField>(-34.0f); // 吸引力: 20.0f
	trunkAttractor->Initialize(p_fngine_);
	trunkAttractor->worldTransform_.Initialize();
	trunkAttractor->worldTransform_.set_.Translation({ 0.0f, 1.0f, 0.0f }); // 幹の中間
	trunkAttractor->radius_ = 10.0f; // 幹の範囲のみ影響
	trunkAttractor->name_ = "TrunkAttractor";
	forceFields_.push_back(std::move(trunkAttractor));

	// 🍃 C. 葉の拡散力 (幹の上部でパーティクルを外側に反発させる)
	std::unique_ptr<PointForceField> foliageRepulsor = std::make_unique<PointForceField>(10.0f); // 反発力: -10.0f
	foliageRepulsor->Initialize(p_fngine_);
	foliageRepulsor->worldTransform_.Initialize();
	foliageRepulsor->worldTransform_.set_.Translation({ 0.0f, 14.0f, 0.0f }); // 葉が広がる高さ
	foliageRepulsor->radius_ = 20.0f; // 拡散範囲
	foliageRepulsor->name_ = "FoliageRepulsor";
	forceFields_.push_back(std::move(foliageRepulsor));
}