//#include "Hear.h"
//#include <d3dx12.h>
//
//void Hear::Initialize(Fngine* engine) {
//	engine_ = engine;
//
//	for (int i = 0; i < 5; ++i) {
//		GuideCurve::ControllerPoint p;
//		// 直線敵なガイドを作成
//		p.position = Vector3(0.0f, 2.0f - (i * 0.5f), 0.0f);
//		p.radius = 0.05f;
//		p.color = Vector3(0.2f, 0.1f, 0.05f);
//		guide_.points_.push_back(p);
//	}
//
//	// ガイドからストランドを生成
//	int numStrands = 10;
//	float spreadRadius = 0.1f;
//	std::vector<Strands::Strand> generateStrands = Strands::GenerateStrandOneGuide(guide_, numStrands, spreadRadius);
//	
//	// dataを一次元化
//	std::vector<Strands::StrandVertex> flatVertices = Strands::FlattenStrands(generateStrands);
//
//	// GPUに送るためのバッファを作成
//	hairAABBBuffer_ = std::make_unique<Structured<Strands::StrandVertex>>(engine);
//	hairAABBBuffer_->Initialize(static_cast<uint32_t>(flatVertices.size()));
//
//	std::memcpy(
//		hairAABBBuffer_->GetMappedData(),
//		flatVertices.data(),
//		sizeof(Strands::StrandVertex) * flatVertices.size()
//	);
//
//	//   ===================
//	// 【 DXR用のAABBの作成 】
//	//   ===================
//
//	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>dxrCmdList;
//	engine->GetCommand().GetList().GetList().As(&dxrCmdList);// 4にキャスト（じゃないとRayTracingが使えない）
//
//	// 1. ジオメトリの設定
//	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc{};
//	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS; // AABBを指定
//	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
//
//	// AABBデータがどこにあるかを設定
//	geometryDesc.AABBs.AABBCount = hairAABBBuffer_->GetNumElements(); // 40個
//	geometryDesc.AABBs.AABBs.StartAddress = hairAABBBuffer_->GetResource()->GetGPUVirtualAddress(); // ※GetResource()があると仮定
//	geometryDesc.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);
//
//	// 2. ビルドの入力設定
//	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS buildInputs{};
//	buildInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL; // BLASを指定
//	buildInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE; // レイの追跡を高速化
//	buildInputs.NumDescs = 1;
//	buildInputs.pGeometryDescs = &geometryDesc;
//
//	// GPUに必要サイズを計算してもらう
//	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo{};
//	auto pDevice = engine->GetD3D12System().GetDevice().Get();
//
//	Microsoft::WRL::ComPtr <ID3D12Device5> pDevice5 = nullptr;
//	HRESULT hr = pDevice->QueryInterface(IID_PPV_ARGS(&pDevice5));
//	if (SUCCEEDED(hr)) {
//		pDevice5->GetRaytracingAccelerationStructurePrebuildInfo(&buildInputs, &prebuildInfo);
//	}
//	else {
//		// そもそもこのパソコン（グラボ）がレイトレ非対応の可能性あり
//	}
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer = CreateBufferResource(
//		pDevice, prebuildInfo.ScratchDataSizeInBytes, true
//	);
//
//	// 最終的なBLASを格納するバッファ（Defaultヒープ、UAV可能である必要があります）
//	Microsoft::WRL::ComPtr<ID3D12Resource> blasResultBuffer = CreateBufferResource(
//		pDevice, prebuildInfo.ResultDataMaxSizeInBytes, true
//	);
//
//	// 4. ビルド命令の発行
//	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc{};
//	buildDesc.Inputs = buildInputs;
//	buildDesc.ScratchAccelerationStructureData = scratchBuffer->GetGPUVirtualAddress();
//	buildDesc.DestAccelerationStructureData = blasResultBuffer->GetGPUVirtualAddress();
//
//	// コマンドリストに積む（実際の計算はGPU側で行われます）
//	dxrCmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);
//
//	// ビルドが完全に終わるまで、後続の処理が待つようにバリアを張る
//	D3D12_RESOURCE_BARRIER uavBarrier{};
//	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
//	uavBarrier.UAV.pResource = blasResultBuffer.Get();
//	dxrCmdList->ResourceBarrier(1, &uavBarrier);
//
//	//   ===========
//	// 【TLASの作成 】
//	//   ===========
//	// --- 1. 配置図（Instance Desc）を1個作る ---
//	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc{};
//
//	// 3x4の行列で位置と向きを指定（今回は回転なし・原点配置の単位行列）
//	instanceDesc.Transform[0][0] = 1.0f; instanceDesc.Transform[1][1] = 1.0f; instanceDesc.Transform[2][2] = 1.0f;
//
//	instanceDesc.InstanceID = 0;                          // シェーダー側で識別するためのID
//	instanceDesc.InstanceMask = 0xFF;                     // すべてのレイにヒットするようにマスクを全通しに
//	instanceDesc.InstanceContributionToHitGroupIndex = 0; // ヒットグループのオフセット
//	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
//
//	// さっき作った「BLAS」のGPUアドレスをここに紐付ける！
//	instanceDesc.AccelerationStructure = blasResultBuffer->GetGPUVirtualAddress();
//
//	// --- 2. 君の Structured クラスにデータを載せる ---
//	// ※今後、複数のキャラクターや髪の束を増やす時は要素数を増やせばOK
//	Structured<D3D12_RAYTRACING_INSTANCE_DESC> instanceBuffer(engine);
//	instanceBuffer.Initialize(1);
//
//	std::memcpy(
//		instanceBuffer.GetMappedData(),
//		&instanceDesc,
//		sizeof(D3D12_RAYTRACING_INSTANCE_DESC)
//	);
//
//	// --- 3. TLASの入力設定 ---
//	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInputs{};
//	tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL; // TLASを指定
//	tlasInputs.Flags = D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_FLAG_PREFER_FAST_TRACE;
//	tlasInputs.NumDescs = 1; // インスタンス（配置図）の数
//	tlasInputs.DescsLayout = D3D12_ELEMENT_LAYOUT_ARRAY;
//	tlasInputs.InstanceProd = instanceBuffer.GetResource()->GetGPUVirtualAddress(); // 配置図のGPUアドレス
//	// --- 4. 必要サイズをGPUに問い合わせ ---
//	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuildInfo{};
//	pDevice5->GetRaytracingAccelerationStructurePrebuildInfo(&tlasInputs, &tlasPrebuildInfo);
//
//	// --- 5. バッファの確保 ---
//	// TLAS用のスクラッチバッファ
//	Microsoft::WRL::ComPtr<ID3D12Resource> tlasScratchBuffer = CreateBufferResource(
//		pDevice, tlasPrebuildInfo.ScratchDataSizeInBytes, true
//	);
//
//	// 最終的なTLASを格納するバッファ（これがレイトレの「世界」そのものになります）
//	Microsoft::WRL::ComPtr<ID3D12Resource> tlasResultBuffer = CreateBufferResource(
//		pDevice, tlasPrebuildInfo.ResultDataMaxSizeInBytes, true
//	);
//
//	// --- 6. 命令の発行 ---
//	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlasBuildDesc{};
//	tlasBuildDesc.Inputs = tlasInputs;
//	tlasBuildDesc.ScratchAccelerationStructureData = tlasScratchBuffer->GetGPUVirtualAddress();
//	tlasBuildDesc.DestAccelerationStructureData = tlasResultBuffer->GetGPUVirtualAddress();
//
//	// コマンドリストに積む
//	dxrCmdList->BuildRaytracingAccelerationStructure(&tlasBuildDesc, 0, nullptr);
//
//	// ビルド完了を待つUAVバリア
//	D3D12_RESOURCE_BARRIER tlasBarrier{};
//	tlasBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
//	tlasBarrier.UAV.pResource = tlasResultBuffer.Get();
//	dxrCmdList->ResourceBarrier(1, &tlasBarrier);
//}
//
//void Hear::Update(float deltaTime) {
//	
//
//}
//
//Microsoft::WRL::ComPtr<ID3D12StateObject> CreateHairRaytracingPSO(
//	ID3D12Device5* device, // DXR対応のDevice5が必要
//	ID3D12RootSignature* globalRootSignature,
//	const void* shaderByteCode,
//	SIZE_T shaderByteCodeSize)
//{
//	// 1. お助けクラス（Descヘルパー）を初期化（コレクション型）
//	CD3DX12_STATE_OBJECT_DESC pipelineDesc(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);
//
//	// 2. DXILライブラリ（コンパイル済みシェーダー）の登録
//	auto libSubobject = pipelineDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
//	libSubobject->SetDXILLibrary(&CD3DX12_SHADER_BYTECODE(shaderByteCode, shaderByteCodeSize));
//
//	// 使用するシェーダー関数名をDXRに登録
//	libSubobject->DefineExport(L"HairIntersectionShader");
//	libSubobject->DefineExport(L"HairClosestHitShader");
//	// ※今回は省略していますが、本来は RayGeneration や Miss シェーダーの名前もここに並べます
//
//	// 3. 【最重要】ヒットグループの作成
//	// 髪の毛は三角形ではないので「PROCEDURAL_PRIMITIVE」を指定します
//	auto hitGroupSubobject = pipelineDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
//	hitGroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);
//	hitGroupSubobject->SetHitGroupExport(L"HairHitGroup"); // このヒットグループの名前
//
//	// 交差シェーダーとヒットシェーダーをこのグループに紐付ける
//	hitGroupSubobject->SetIntersectionShaderImport(L"HairIntersectionShader");
//	hitGroupSubobject->SetClosestHitShaderImport(L"HairClosestHitShader");
//
//	// 4. シェーダー設定（データサイズの申告）
//	auto shaderConfigSubobject = pipelineDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
//
//	// ペイロードサイズ: RayPayload（float3 color = 12バイト）
//	UINT payloadSize = sizeof(float) * 3;
//	// アトリビュートサイズ: Intersectionから渡される float2 attr（8バイト）
//	UINT attributeSize = sizeof(float) * 2;
//	shaderConfigSubobject->Config(payloadSize, attributeSize);
//
//	// 5. グローバルルートシグネチャの紐付け
//	auto globalRootSigSubobject = pipelineDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
//	globalRootSigSubobject->SetRootSignature(globalRootSignature);
//
//	// 6. パイプライン設定（レイの再帰の深さ）
//	auto pipelineConfigSubobject = pipelineDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
//	// 一次レイ（カメラからの光線）だけなら「1」でOK。鏡や影を作るなら増やします
//	pipelineConfigSubobject->Config(1);
//
//	// --- ついにビルド！ ---
//	Microsoft::WRL::ComPtr<ID3D12StateObject> rtpso;
//	device->CreateStateObject(pipelineDesc, IID_PPV_ARGS(&rtpso));
//
//	return rtpso;
//}
//
//// 各種サイズ定義（DX12の制約に合わせるアラインメント計算）
//constexpr UINT AlignTo(UINT size, UINT alignment) {
//	return (size + alignment - 1) & ~(alignment - 1);
//}
//
//void CreateShaderBindingTable(
//	ID3D12StateObject* rtpso,
//	ID3D12Device* device,
//	Microsoft::WRL::ComPtr<ID3D12Resource>& sbtBuffer,
//	D3D12_DISPATCH_RAYS_DESC& dispatchDesc) // 後で描画に使う設定をここに書き込む
//{
//	// 1. PSOから「シェーダー識別子を取り出すためのインターフェース」をクエリする
//	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtpsoProps;
//	rtpso->QueryInterface(IID_PPV_ARGS(&rtpsoProps));
//
//	// 各シェーダーの識別子（一辺32バイトの固有データ）を取得
//	void* rayGenId = rtpsoProps->GetShaderIdentifier(L"MyRayGenShader");    // あなたのRayGen名
//	void* missId = rtpsoProps->GetShaderIdentifier(L"MyMissShader");      // あなたのMiss名
//	void* hairHitId = rtpsoProps->GetShaderIdentifier(L"HairHitGroup");      // さっき登録した髪のグループ名
//
//	// 2. サイズの計算（32バイトアラインメントを適用）
//	UINT shaderIdSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES; // 常に32バイト
//
//	UINT rayGenRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32
//	UINT missRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32
//	UINT hitGroupRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32
//
//	// 各テーブルの開始位置（オフセット）を計算（64バイトアラインメントを適用）
//	UINT rayGenSectionSize = AlignTo(rayGenRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
//	UINT missSectionSize = AlignTo(missRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
//	UINT hitGroupSectionSize = AlignTo(hitGroupRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
//
//	UINT totalSBTSize = rayGenSectionSize + missSectionSize + hitGroupSectionSize;
//
//	// 3. バッファの作成（Uploadヒープで作成）
//	sbtBuffer = CreateBufferResource(device, totalSBTSize);
//
//	// 4. メモリにシェーダー識別子を書き込む
//	uint8_t* pData = nullptr;
//	sbtBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
//
//	// 内部を一旦ゼロクリア
//	std::memset(pData, 0, totalSBTSize);
//
//	// 各セクションのポインタを計算
//	uint8_t* pRayGenHeader = pData;
//	uint8_t* pMissHeader = pRayGenHeader + rayGenSectionSize;
//	uint8_t* pHitGroupHeader = pMissHeader + missSectionSize;
//
//	// 識別子をコピー（memcpy）
//	std::memcpy(pRayGenHeader, rayGenId, shaderIdSize);
//	std::memcpy(pMissHeader, missId, shaderIdSize);
//	std::memcpy(pHitGroupHeader, hairHitId, shaderIdSize);
//
//	sbtBuffer->Unmap(0, nullptr);
//
//	// 5. 【超重要】描画命令（DispatchRays）に渡すための設定（Desc）を埋める
//	D3D12_GPU_VIRTUAL_ADDRESS sbtAddress = sbtBuffer->GetGPUVirtualAddress();
//
//	// RayGenの設定
//	dispatchDesc.RayGenerationShaderRecord.StartAddress = sbtAddress;
//	dispatchDesc.RayGenerationShaderRecord.SizeInBytes = rayGenRecordSize;
//
//	// Missの設定
//	dispatchDesc.MissShaderTable.StartAddress = sbtAddress + rayGenSectionSize;
//	dispatchDesc.MissShaderTable.SizeInBytes = missRecordSize;
//	dispatchDesc.MissShaderTable.StrideInBytes = missRecordSize;
//
//	// HitGroupの設定（ここに髪の毛の起動ボタンの情報が入る）
//	dispatchDesc.HitGroupTable.StartAddress = sbtAddress + rayGenSectionSize + missSectionSize;
//	dispatchDesc.HitGroupTable.SizeInBytes = hitGroupRecordSize;
//	dispatchDesc.HitGroupTable.StrideInBytes = hitGroupRecordSize;
//}
//
//void Hear::Render() {
//	auto commandList = engine_->GetCommand().GetList().GetList().Get();
//
//	// --- 1. グローバルルートシグネチャの設定 ---
//	// レイトレ全体で使う共通のバッファ（TLASや出力テクスチャなど）をシェーダーに紐付けます
//	commandList->SetComputeRootSignature(globalRootSignature.Get());
//
//	// [スロット0]: あなたがビルドした「TLAS（世界）」のGPUアドレスを渡す
//	commandList->SetComputeRootShaderResourceView(0, tlasResultBuffer->GetGPUVirtualAddress());
//
//	// [スロット1]: 君の作った StructuredBuffer から「髪の頂点データ」のSRVを渡す
//	commandList->SetComputeRootDescriptorTable(1, hairVertexBuffer.GetSRVHandleGPU());
//
//	// [スロット2]: 画面にレンダリング結果を書き込むための「出力テクスチャ（UAV）」を渡す
//	// ※Fngineのレンダーターゲット（UAV対応）のアドレスを指定
//	commandList->SetComputeRootDescriptorTable(2, renderTargetUAVHandleGPU);
//
//
//	// --- 2. パイプライン（RTPSO）のセット ---
//	// コマンドリストにレイトレ専用パイプラインを設定（StateObjectをセットします）
//	dxrCmdList->SetPipelineState1(rtpso.Get());
//
//
//	// --- 3. DXRレイトレーシングの実行！ ---
//	// 前ステップで SBT の情報を詰め込んだ `dispatchDesc` に、画面サイズを設定します
//	dispatchDesc.Width = windowWidth;  // 画面の横幅（例: 1920）
//	dispatchDesc.Height = windowHeight; // 画面の縦幅（例: 1080）
//	dispatchDesc.Depth = 1;            // 2D画面なので1でOK
//
//	// GPUにレイトレーシングの開始を命令！
//	dxrCmdList->DispatchRays(&dispatchDesc);
//}