#include "IHair.h"
#include <d3dx12.h>
#include "CameraSystem.h"
#include "../Engine/Objects/Primitive/Box/PrimitiveBox.h"

// CPPのみで定義されている関数

// 各種サイズ定義（DX12の制約に合わせるアラインメント計算）
constexpr UINT AlignTo(UINT size, UINT alignment) {
	return (size + alignment - 1) & ~(alignment - 1);
}

void CreateShaderBindingTable(
	ID3D12StateObject* rtpso,
	ID3D12Device* device,
	Microsoft::WRL::ComPtr<ID3D12Resource>& sbtBuffer,
	D3D12_DISPATCH_RAYS_DESC& dispatchDesc) // 後で描画に使う設定をここに書き込む
{

	// -----------------------------------------------

		// PSOから「シェーダー識別子を取り出すためのインターフェース」をクエリする
	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtpsoProps;
	rtpso->QueryInterface(IID_PPV_ARGS(&rtpsoProps));

	// 各シェーダーの識別子（一辺32バイトの固有データ）を取得
	void* rayGenId = rtpsoProps->GetShaderIdentifier(L"MyRayGenShader");    // あなたのRayGen名
	void* missId = rtpsoProps->GetShaderIdentifier(L"MyMissShader");      // あなたのMiss名
	void* hairHitId = rtpsoProps->GetShaderIdentifier(L"HairHitGroup");      // さっき登録した髪のグループ名

	// -----------------------------------------------

		// GPUが分かりやすいようにメモリを分けて配置。
		// サイズの計算（32バイトアラインメントを適用）
	UINT shaderIdSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES; // 常に32バイト

	UINT rayGenRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32
	UINT missRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32
	UINT hitGroupRecordSize = AlignTo(shaderIdSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // 32

	// 各テーブルの開始位置（オフセット）を計算（64バイトアラインメントを適用）
	UINT rayGenSectionSize = AlignTo(rayGenRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
	UINT missSectionSize = AlignTo(missRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);
	UINT hitGroupSectionSize = AlignTo(hitGroupRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

	UINT totalSBTSize = rayGenSectionSize + missSectionSize + hitGroupSectionSize;

	// -----------------------------------------------

		// 3. バッファの作成（Uploadヒープで作成）
	sbtBuffer = CreateBufferResource(device, totalSBTSize);

	// 4. メモリにシェーダー識別子を書き込む
	uint8_t* pData = nullptr;
	sbtBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));

	// 内部を一旦ゼロクリア（dataが残っている可能性をゼロにするため）
	std::memset(pData, 0, totalSBTSize);

	// 各セクションのポインタを計算
	uint8_t* pRayGenHeader = pData;
	uint8_t* pMissHeader = pRayGenHeader + rayGenSectionSize;
	uint8_t* pHitGroupHeader = pMissHeader + missSectionSize;

	// 識別子をコピー（memcpy）
	std::memcpy(pRayGenHeader, rayGenId, shaderIdSize);
	std::memcpy(pMissHeader, missId, shaderIdSize);
	std::memcpy(pHitGroupHeader, hairHitId, shaderIdSize);

	sbtBuffer->Unmap(0, nullptr);

	// -----------------------------------------------

		// 5. 【超重要】描画命令（DispatchRays）に渡すための設定（Desc）を埋める
	D3D12_GPU_VIRTUAL_ADDRESS sbtAddress = sbtBuffer->GetGPUVirtualAddress();

	// RayGenの設定(一描画につき一回しか使わないためTableでなくていい)
	dispatchDesc.RayGenerationShaderRecord.StartAddress = sbtAddress;
	dispatchDesc.RayGenerationShaderRecord.SizeInBytes = rayGenRecordSize;

	// Missの設定
	dispatchDesc.MissShaderTable.StartAddress = sbtAddress + rayGenSectionSize;
	dispatchDesc.MissShaderTable.SizeInBytes = missRecordSize;
	dispatchDesc.MissShaderTable.StrideInBytes = missRecordSize;

	// HitGroupの設定（ここに髪の毛の起動ボタンの情報が入る）
	dispatchDesc.HitGroupTable.StartAddress = sbtAddress + rayGenSectionSize + missSectionSize;
	dispatchDesc.HitGroupTable.SizeInBytes = hitGroupRecordSize;
	dispatchDesc.HitGroupTable.StrideInBytes = hitGroupRecordSize;
}

Microsoft::WRL::ComPtr<IDxcBlob> CompileRaytracingShader(
	const std::wstring& filePath,
	const wchar_t* profile, // ここには L"lib_6_5" などを渡します
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	Log::View(ConvertString(std::format(L"Begin DXR CompileShader, path:{}, profile:{}\n", filePath, profile)));

	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	// 【注目！】レイトレ（ライブラリ）用のコンパイル引数
	std::vector<LPCWSTR> arguments = {
		filePath.c_str(),
		L"-T", profile,          // L"lib_6_5" などを指定
		L"-HV", L"2021",         // float32_t 等を使うためにHLSL 2021規格を有効化
		L"-Zi", L"-Qembed_debug",// デバッグ用
		L"-Od",                  // 最適化オフ（デバッグしやすくする）
		L"-Zpr",                 // 行優先（Row Major）
	};

	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments.data(),
		static_cast<UINT32>(arguments.size()),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(hr));

	// エラーチェック
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log::View(shaderError->GetStringPointer());
		assert(false); // 警告・エラーがあればここで止まる
	}

	// 実行用バイナリの取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	Log::View(ConvertString(std::format(L"DXR CompileSucceeded, path:{}\n", filePath)));

	shaderSource->Release();
	shaderResult->Release();

	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12StateObject> Hair::CreateHairRaytracingPSO(
	ID3D12Device5* device, // DXR対応のDevice5が必要
	ID3D12RootSignature* globalRootSignature,
	const void* shaderByteCode,
	SIZE_T shaderByteCodeSize)
{
	// -----------------------------------------------

		// お助けクラス（Descヘルパー）を初期化
	CD3DX12_STATE_OBJECT_DESC pipelineDesc(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

	// -----------------------------------------------

		// DXILライブラリ（コンパイル済みシェーダー）の登録
	auto libSubobject = pipelineDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
	D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE(shaderByteCode, shaderByteCodeSize);
	libSubobject->SetDXILLibrary(&libdxil);

	// 使用するシェーダー関数名をDXRに登録 -> ShaderImportで登録
	libSubobject->DefineExport(L"MyRayGenShader");
	libSubobject->DefineExport(L"MyMissShader");
	libSubobject->DefineExport(L"HairIntersectionShader");
	libSubobject->DefineExport(L"HairClosestHitShader");

	// -----------------------------------------------

		// 【最重要】ヒットグループの作成
	auto hitGroupSubobject = pipelineDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
	// 髪の毛は三角形ではないので「PROCEDURAL_PRIMITIVE」を指定します
	hitGroupSubobject->SetHitGroupType(D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);
	// このヒットグループの名前を設定
	hitGroupSubobject->SetHitGroupExport(L"HairHitGroup");

	// 交差シェーダーとヒットシェーダーをこのグループに紐付ける
	hitGroupSubobject->SetIntersectionShaderImport(L"HairIntersectionShader");
	hitGroupSubobject->SetClosestHitShaderImport(L"HairClosestHitShader");

	// -----------------------------------------------

		// シェーダー間を行き来するデータ量の設定（データサイズの申告）
	auto shaderConfigSubobject = pipelineDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();

	// ペイロードサイズ: RayPayload（float3 color = 12バイト）
	UINT payloadSize = sizeof(float) * 3;
	// アトリビュートサイズ: Intersectionから渡される float2 attr（8バイト）
	UINT attributeSize = sizeof(float) * 5;
	shaderConfigSubobject->Config(payloadSize, attributeSize);

	// -----------------------------------------------

		// グローバルルートシグネチャの紐付け
	auto globalRootSigSubobject = pipelineDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSigSubobject->SetRootSignature(globalRootSignature);

	// -----------------------------------------------

		// パイプライン設定（レイの再帰の深さ）
	auto pipelineConfigSubobject = pipelineDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	// 一次レイ（カメラからの光線）だけなら「1」でOK。鏡や影を作るなら増やします
	pipelineConfigSubobject->Config(1);

	// -----------------------------------------------

		// --- ついにビルド！ ---
	Microsoft::WRL::ComPtr<ID3D12StateObject> rtpso;
	device->CreateStateObject(pipelineDesc, IID_PPV_ARGS(&rtpso));

	return rtpso;
}

// -----------------------------------------------

void Hair::Initialize(Fngine* engine) {
// -----------------------------------------------

	// engineの情報を取得
	engine_ = engine;

// -----------------------------------------------

	// 【 RayTracingを使用するために4にキャスト 】
	engine->GetCommand().GetList().GetList().As(&dxrCmdList_);

// -----------------------------------------------

	// キャンバスを作成
	outputTexture_ = std::make_unique<RWTexture2D>();
	// フォーマットはエンジンのスワップチェーンに合わせる (例: DXGI_FORMAT_R8G8B8A8_UNORM)
	outputTexture_->Initialize(engine, 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	outputTexture_->GetResource()->SetName(L"HairOutputTexture");

	// DXRに渡すためのGPUハンドルをメンバ変数に保存！
	renderTargetUAVHandleGPU_ = outputTexture_->GetUAVHandleGPU();

// -----------------------------------------------

	//   ================
	// 【 物理処理の設定 】
	//   ================
	GuideCurve::HairPhysicsConfig commonPhysics;
	commonPhysics.stiffness = 0.4f;
	commonPhysics.restoringForce = 0.2f;
	commonPhysics.damping = 0.96f;
	commonPhysics.padding = 0.0f;

	gpuPhysicsConfigBuffer_ = std::make_unique<ConstantBuffer<GuideCurve::HairPhysicsConfig>>(engine);
	gpuPhysicsConfigBuffer_->Initialize();
	gpuPhysicsConfigBuffer_->GetMappedData()->stiffness = commonPhysics.stiffness;
	gpuPhysicsConfigBuffer_->GetMappedData()->restoringForce = commonPhysics.restoringForce;
	gpuPhysicsConfigBuffer_->GetMappedData()->damping = commonPhysics.damping;
	gpuPhysicsConfigBuffer_->GetMappedData()->padding = commonPhysics.padding;

	GuideCurve::FrameConfig frameConfig;
	frameConfig.gravity = { 0.0f, 0.0f,0.0f };
	frameConfig.deltaTime = 1.0f / 60.0f;
	frameConfig.windDirection = { -0.0f,0.0f,0.0f };
	frameConfig.time = 0.0f;
	frameConfig.moveDirection = { 0.0f,0.0f,0.0f };
	frameConfig.padding = 0.0f;

	gpuFrameConfigBuffer_ = std::make_unique<ConstantBuffer<GuideCurve::FrameConfig>>(engine);
	gpuFrameConfigBuffer_->Initialize();
	std::memcpy(gpuFrameConfigBuffer_->GetMappedData(), &frameConfig, sizeof(GuideCurve::FrameConfig));

	//   ==================
	// 【 髪の基本的な設定 】
	//   ==================
	Strands::HairConfig hairConfig;
	hairConfig.numGuides = 1000;           // ガイドの総本数
	hairConfig.pointPerGuide = 16;      // ガイド一本を作る数
	hairConfig.pointPerStrand = 32;     // ストランド一本を作る数
	hairConfig.numStrands = 10000.0f;     // ストランドの総本数

	gpuConfigBuffer_ = std::make_unique<ConstantBuffer<Strands::HairConfig>>(engine);
	gpuConfigBuffer_->Initialize();
	gpuConfigBuffer_->GetMappedData()->numGuides = hairConfig.numGuides;
	gpuConfigBuffer_->GetMappedData()->pointPerGuide = hairConfig.pointPerGuide;
	gpuConfigBuffer_->GetMappedData()->pointPerStrand = hairConfig.pointPerStrand;
	gpuConfigBuffer_->GetMappedData()->numStrands = hairConfig.numStrands;

	Strands::HairMakeConfig makeConfig;
	makeConfig.globalSpreadRadius = 0.05f;//
	makeConfig.globalHairThickness = 1.0f;
	makeConfig.paddings[0] = 0.0f;
	makeConfig.paddings[1] = 0.0f;

	gpuMakeConfigBuffer_ = std::make_unique<ConstantBuffer<Strands::HairMakeConfig>>(engine);
	gpuMakeConfigBuffer_->Initialize();
	std::memcpy(gpuMakeConfigBuffer_->GetMappedData(), &makeConfig, sizeof(Strands::HairMakeConfig));

	//   ==============
	// 【 ガイドの作成 】
	//   ==============
	float headRadius = 0.05f;// 範囲
	float segmentLength = 0.01f;// 点間の距離

#pragma region 初期Hair
	// 本当ならガイドの根本の情報を取得して生やす(pos, direction)
	for (UINT g = 0; g < hairConfig.numGuides; ++g) {
		GuideCurve::GuideHear guide;
		float guideAngle = Deg2Rad((360.0f / hairConfig.numGuides) * g);
		Vector3 rootPosition(std::cosf(guideAngle) * headRadius, 0.0f, std::sinf(guideAngle) * headRadius);

		for (UINT i = 0; i < hairConfig.pointPerGuide; ++i) {
			GuideCurve::ControllerPoint cp;
			// 外側に向かって少し広がりながら垂れ下がる初期ポーズ
			Vector3 offsetDir(std::cosf(guideAngle) * 0.02f * static_cast<float>(i), 0.0f, std::sinf(guideAngle) * 0.02f * static_cast<float>(i));
			cp.position = rootPosition + Vector3(0.0f, -static_cast<float>(i) * segmentLength, 0.0f) + offsetDir;
			cp.homePosition = cp.position;
			cp.radius = 0.0025f * (1.0f - static_cast<float>(i) / hairConfig.pointPerGuide); // 先細り
			//cp.radius = 0.05f;

			cp.color = Vector3(1.0f - (0.04f * i), 1.0f - (0.04f * i), 1.0f);
			//cp.color = Vector3(1.0f,1.0f,1.0f);
			cp.nextToLength = segmentLength;

			// 根元固定、毛先ほど動く
			cp.physicsWeight = static_cast<float>(i) / static_cast<float>(hairConfig.pointPerGuide - 1);
			cp.physicsWeight = 0.5f;
			if (i == 0) cp.physicsWeight = 0.0f;

			guide.points.push_back(cp);
		}
		hairData_.guides.push_back(guide);
	}

#pragma endregion
	
	// 全ガイドの全制御点を「1つのフラットな配列」に結合してGPUへ送る
	std::vector<GuideCurve::ControllerPoint> flatGuidePoints;
	for (const auto& g : hairData_.guides) {
		for (const auto& p : g.points) {
			flatGuidePoints.push_back(p);
		}
	}
	// UAVの作成
	gpuGuideBuffer_ = std::make_unique<RWStructured<GuideCurve::ControllerPoint>>(engine);
	gpuGuideBuffer_->Initialize(static_cast<UINT>(flatGuidePoints.size()));

// -----------------------------------------------

	// dataをコピーするためだけに作成
	uploadGuideBuffer = std::make_unique<Structured<GuideCurve::ControllerPoint>>(engine);
	uploadGuideBuffer->Initialize(static_cast<UINT>(flatGuidePoints.size()));
	std::memcpy(uploadGuideBuffer->GetMappedData(), flatGuidePoints.data(), sizeof(GuideCurve::ControllerPoint) * flatGuidePoints.size());

	// コピー先に遷移
	auto transitionToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
		gpuGuideBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);

	// バリア開始
	D3D12_RESOURCE_BARRIER barriers[] = { transitionToCopyDest};
	dxrCmdList_->ResourceBarrier(1, barriers);
	
	// コピー開始
	dxrCmdList_->CopyResource(gpuGuideBuffer_->GetResource(), uploadGuideBuffer->GetResource());

	// コピーが終わったら、UAVバッファを「UAVとして使える状態」に戻す
	auto transitionToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
		gpuGuideBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	dxrCmdList_->ResourceBarrier(1, &transitionToUAV);

	// ガイドの総本数 = 全頂点数 / 16
	uint32_t totalGuides = gpuGuideBuffer_->GetNumElements() / hairConfig.pointPerGuide;
	uint32_t physicsGroupX = (totalGuides + 63) / 64;

	//   =====================
	// 【 HairGuideの物理演算 】
	//   =====================
	ID3D12DescriptorHeap* ppHeaps[] = { engine_->GetSRV().GetDescriptorHeap().GetHeap().Get()};
	// コマンドリストにヒープをセット（テーブルをセットするより【前】に呼ぶ！）
	dxrCmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);


	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairPhysicsCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairPhysicsCS").GetCPS().Get());
	
	dxrCmdList_->SetComputeRootConstantBufferView(0, gpuPhysicsConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootConstantBufferView(1, gpuFrameConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootDescriptorTable(2, gpuGuideBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(3, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(physicsGroupX, 1, 1);

	auto barrierGuide = CD3DX12_RESOURCE_BARRIER::Transition(
		gpuGuideBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	dxrCmdList_->ResourceBarrier(1, &barrierGuide);

// -----------------------------------------------

	//   ================
	// 【 ストランド設計 】
	//   ================
	std::vector<Strands::ChildStrand> childStrands;
	auto rand = RandomUtils::GetInstance();
	float spreadRadius = 0.005f;
	UINT NUM_STRANDS = static_cast<UINT>(hairConfig.numStrands);

	for (UINT s = 0; s < NUM_STRANDS; ++s) {
		Strands::ChildStrand child;

		if (s < 300) {
			uint32_t targetGuideId = rand->GetHighRandom().GetInt(0, hairConfig.numGuides - 1); // ランダムに1本選ぶ
			child.parentGuideIds[0] = targetGuideId;
			child.parentGuideIds[1] = targetGuideId;
			child.parentGuideIds[2] = targetGuideId;
			child.blendMode = 1; // 単一ガイド追従モード//

			child.weights[0] = 1.0f;
			child.weights[1] = 0.0f;
			child.weights[2] = 0.0f;

			child.twistAngle = Deg2Rad(1080.0f); // 3回転ドリルねじれ
			child.clumpForce = 0.8f;             // ギュッと束ねる
		}
		// 隣り合う2本の親ガイドを滑らかに補間する「ブレンド髪」にする
		else {
			uint32_t guideA = rand->GetHighRandom().GetInt(0, hairConfig.numGuides - 2);
			uint32_t guideB = guideA + 1; // 隣のガイド

			child.parentGuideIds[0] = guideA;
			child.parentGuideIds[1] = guideB;
			child.parentGuideIds[2] = guideB;
			child.blendMode = 1; // 複数ガイドブレンドモード

			float w = rand->GetHighRandom().GetFloat(0.1f, 0.9f); // 2本の間でのランダムな位置
			child.weights[0] = w;
			child.weights[1] = 1.0f - w;
			child.weights[2] = 0.0f;

			child.twistAngle = 0.0f; // ブレンド髪はねじらないストレート
			child.clumpForce = 0.2f; // ゆるめの束感
		}

		//child.lengthScale = 0.9f + rand->GetHighRandom().GetFloat(0.0f, 0.2f);
		child.lengthScale = 1.0f;

		// ガイドの周りに散らすオフセット
		float angle = static_cast<float>(rand->GetHighRandom().GetInt(0, 360));
		float r = sqrtf(rand->GetHighRandom().GetFloat(0.0f, 1.0f)) * spreadRadius;
		child.offset = Vector2(std::cosf(Deg2Rad(angle)) * r, std::sinf(Deg2Rad(angle)) * r);

		child.seed = s; // チラつき防止固定シード
		child.waveAmplitude = 0.01f;
		child.waveFrequency = 4.0f;
		child.noise = 0.0f;

		childStrands.push_back(child);
	}

	gpuChildStrandBuffer_ = std::make_unique<Structured<Strands::ChildStrand>>(engine);
	gpuChildStrandBuffer_->Initialize(static_cast<UINT>(childStrands.size()));
	std::memcpy(gpuChildStrandBuffer_->GetMappedData(), childStrands.data(), sizeof(Strands::ChildStrand) * childStrands.size());

	UINT totalVertices = NUM_STRANDS * hairConfig.pointPerStrand;

	// GPUに送るためのバッファを作成
	hairVertexBuffer_ = std::make_unique<RWStructured<Strands::StrandVertex>>(engine);
	hairVertexBuffer_->Initialize(totalVertices);
	hairVertexBuffer_->GetResource()->SetName(L"HairVertexBuffer");

	uint32_t generateGroupX = (NUM_STRANDS + 63) / 64;

	// Strands作成CSを実行
	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairGenerateCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairGenerateCS").GetCPS().Get());
	
	dxrCmdList_->SetComputeRootDescriptorTable(0, gpuGuideBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(1, gpuChildStrandBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(2, hairVertexBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(3, gpuMakeConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootConstantBufferView(4, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(generateGroupX, 1, 1);

	// 書き込み完了を待つ
	auto barrierVertices = CD3DX12_RESOURCE_BARRIER::Transition(
		hairVertexBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	dxrCmdList_->ResourceBarrier(1, &barrierVertices);

	/////////////////////////////
	//
	//  AABBBufferについての処理
	//
	/////////////////////////////
	hairAABBBuffer_ = std::make_unique<RWStructured<D3D12_RAYTRACING_AABB>>(engine);
	// ここのサイズをどうしよう。多めに取るのが正解かな？
	hairAABBBuffer_->Initialize(NUM_STRANDS * (hairConfig.pointPerStrand - 1));
	hairAABBBuffer_->GetResource()->SetName(L"HairAABBBuffer");

	// 計算を実行
	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairAABBCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairAABBCS").GetCPS().Get());
	// ここにRootSignatureのやつを設定していく
	
	dxrCmdList_->SetComputeRootDescriptorTable(0, hairVertexBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(1, hairAABBBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(2, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(generateGroupX, 1, 1);

	D3D12_RESOURCE_BARRIER uavBarrier = {};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = hairAABBBuffer_->GetResource(); // AABBバッファ
	dxrCmdList_->ResourceBarrier(1, &uavBarrier);

	// 作業完了を待つ
	auto barrierAABB = CD3DX12_RESOURCE_BARRIER::Transition(
		hairAABBBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	dxrCmdList_->ResourceBarrier(1, &barrierAABB);
	
	Log::View("AABB Compleate");
	// AABBが完成しました。。。

// -----------------------------------------------

	//   ===================
	// 【 DXR用のAABBの作成 】
	//   ===================
#pragma region BLASの作成のセット

	//   ==============================
	// 【 BLASの構造体を作るための情報 】
	//   ==============================
	// ジオメトリの設定
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc{};
	// ジオメトリの種類
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS; // AABBを指定
	// ジオメトリのフラグ
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

	// 【 AABBの数を設置場所 】
	geometryDesc.AABBs.AABBCount = hairAABBBuffer_->GetNumElements();
	geometryDesc.AABBs.AABBs.StartAddress = hairAABBBuffer_->GetResource()->GetGPUVirtualAddress();
	geometryDesc.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);

	//   ======================
	// 【 BLASを作るための情報 】
	//   ======================
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS buildInputs{};
	// Type : 構築する加速度構造体の種類
	buildInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL; // BLASを指定
	// Flags : フラグ
	buildInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE
						| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE; // レイの追跡を高速化
	/*switch (Type) {
		case D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TOP_LEVEL
			DescsLayout に基づいてレイアウトされたインスタンスの数
		case D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BOTTOM_LEVEL
			pGeometryDescs または ppGeometryDescs によって参照される要素の数
	}*/
	buildInputs.NumDescs = 1;

	// BLASの構造体を作るための情報をセッティング
	buildInputs.pGeometryDescs = &geometryDesc;
#pragma endregion

// -----------------------------------------------

	auto pDevice = engine->GetD3D12System().GetDevice().Get();
	Microsoft::WRL::ComPtr <ID3D12Device5> pDevice5 = nullptr;
	HRESULT hr = pDevice->QueryInterface(IID_PPV_ARGS(&pDevice5));

// -----------------------------------------------

	//   =====================================================
	// 【 GPUにBLASをビルドするのに必要サイズを計算してもらう 】
	//   =====================================================
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo{};

	if (SUCCEEDED(hr)) {
		// ビルドした後に必要なサイズ、ビルドするのにかかるサイズ、其のあとに調整するときにかかるサイズ
		pDevice5->GetRaytracingAccelerationStructurePrebuildInfo(&buildInputs, &prebuildInfo);
	}
	else {
		// そもそもこのパソコン（グラボ）がレイトレ非対応の可能性あり
	}

	// -----------------------------------------------

		// 作業用のBuffer
	scratchBuffer_ = CreateBufferResource(
		pDevice, prebuildInfo.ScratchDataSizeInBytes, true
	);
	scratchBuffer_->SetName(L"BLAS Scratch Buffer");

	D3D12_HEAP_PROPERTIES blasHeapProps{};
	blasHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC blasResDesc{};
	blasResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	blasResDesc.Width = prebuildInfo.ResultDataMaxSizeInBytes; // 必要なサイズ
	blasResDesc.Height = 1;
	blasResDesc.DepthOrArraySize = 1;
	blasResDesc.MipLevels = 1;
	blasResDesc.Format = DXGI_FORMAT_UNKNOWN;
	blasResDesc.SampleDesc.Count = 1;
	blasResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	blasResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; // UAVフラグ

	pDevice->CreateCommittedResource(
		&blasHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&blasResDesc,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, // ★初期状態を最初からこれにする！
		nullptr,
		IID_PPV_ARGS(&blasResultBuffer_)
	);

	blasResultBuffer_->SetName(L"BLAS Result Buffer");

// -----------------------------------------------

	//   ==================
	// 【 ビルド命令の発行 】
	//   ==================
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc{};
	// ビルドの構築設定
	buildDesc.Inputs = buildInputs;
	// ビルドする場所
	buildDesc.ScratchAccelerationStructureData = scratchBuffer_->GetGPUVirtualAddress();
	// ビルド後の保存場所
	buildDesc.DestAccelerationStructureData = blasResultBuffer_->GetGPUVirtualAddress();

// -----------------------------------------------

	// コマンドリストに積む
	dxrCmdList_->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

// -----------------------------------------------

	// ビルドが完全に終わるまで、後続の処理が待つようにバリアを張る
	D3D12_RESOURCE_BARRIER uavAABBBarrier{};
	uavAABBBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavAABBBarrier.UAV.pResource = blasResultBuffer_.Get();
	dxrCmdList_->ResourceBarrier(1, &uavAABBBarrier);

// -----------------------------------------------

#pragma region TLASの作成のセット
	//   ==============================
	// 【 TLASの構造体を作るための情報 】
	//   ==============================
	// --- 1. 配置図（Instance Desc）を1個作る ---
	D3D12_RAYTRACING_INSTANCE_DESC instanceDesc{};

	// 3x4の行列で位置と向きを指定（今回は回転なし・原点配置の単位行列）
	instanceDesc.Transform[0][0] = 1.0f;
	instanceDesc.Transform[1][1] = 1.0f;
	instanceDesc.Transform[2][2] = 1.0f;
	// シェーダー側で識別するためのID
	instanceDesc.InstanceID = 0;
	// どのレイにヒットするかのマスク設定
	instanceDesc.InstanceMask = 0xFF;
	// ヒットグループのオフセット
	instanceDesc.InstanceContributionToHitGroupIndex = 0;
	// オブジェクトの設定
	instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
	// さっき作った「BLAS」のGPUアドレスをここに紐付ける！
	instanceDesc.AccelerationStructure = blasResultBuffer_->GetGPUVirtualAddress();

	// --- 2. 君の Structured クラスにデータを載せる ---
	// ※今後、複数のキャラクターや髪の束を増やす時は要素数を増やせばOK
	instanceBuffer = std::make_unique<Structured<D3D12_RAYTRACING_INSTANCE_DESC>>(engine);
	instanceBuffer->Initialize(1);
	instanceBuffer->GetResource()->SetName(L"TLAS Instance Buffer");

	std::memcpy(
		instanceBuffer->GetMappedData(),
		&instanceDesc,
		sizeof(D3D12_RAYTRACING_INSTANCE_DESC)
	);

	//   ======================
	// 【 TLASを作るための情報 】
	//   ======================
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInputs{};
	// TLASを指定
	tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	// 作り方の指定
	tlasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE
						| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	// BLASの総数
	tlasInputs.NumDescs = 1; // インスタンス（配置図）の数
	// dataの並び方
	tlasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	// BLASのGPUアドレス
	tlasInputs.InstanceDescs = instanceBuffer->GetResource()->GetGPUVirtualAddress();
#pragma endregion

// -----------------------------------------------

	//   =====================================================
	// 【 GPUにTLASをビルドするのに必要サイズを計算してもらう 】
	//   =====================================================
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuildInfo{};
	pDevice5->GetRaytracingAccelerationStructurePrebuildInfo(&tlasInputs, &tlasPrebuildInfo);

// -----------------------------------------------

	// TLAS用のスクラッチバッファ（作業場所の確保）
	tlasScratchBuffer_ = CreateBufferResource(
		pDevice, tlasPrebuildInfo.ScratchDataSizeInBytes, true
	);
	tlasScratchBuffer_->SetName(L"TLAS Scratch Buffer");

	// 最終的なTLASを格納するバッファ（これがレイトレの「世界」そのもの）
	//tlasResultBuffer_ = CreateBufferResource(
	//	pDevice, tlasPrebuildInfo.ResultDataMaxSizeInBytes, true
	//);
	
	D3D12_HEAP_PROPERTIES tlasHeapProps{};
	tlasHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC tlasResDesc{};
	tlasResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tlasResDesc.Width = tlasPrebuildInfo.ResultDataMaxSizeInBytes; // 必要なサイズ
	tlasResDesc.Height = 1;
	tlasResDesc.DepthOrArraySize = 1;
	tlasResDesc.MipLevels = 1;
	tlasResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tlasResDesc.SampleDesc.Count = 1;
	tlasResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tlasResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; // UAVフラグ

	pDevice->CreateCommittedResource(
		&tlasHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&tlasResDesc,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		nullptr,
		IID_PPV_ARGS(&tlasResultBuffer_)
	);

	tlasResultBuffer_->SetName(L"TLAS Result Buffer");


// -----------------------------------------------

	//   ==================
	// 【 ビルド命令の発行 】
	//   ==================
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlasBuildDesc{};
	tlasBuildDesc.Inputs = tlasInputs;
	tlasBuildDesc.ScratchAccelerationStructureData = tlasScratchBuffer_->GetGPUVirtualAddress();
	tlasBuildDesc.DestAccelerationStructureData = tlasResultBuffer_->GetGPUVirtualAddress();

// -----------------------------------------------

	// コマンドリストに積む
	dxrCmdList_->BuildRaytracingAccelerationStructure(&tlasBuildDesc, 0, nullptr);

// -----------------------------------------------

	// ビルド完了を待つUAVバリア
	D3D12_RESOURCE_BARRIER tlasBarrier{};
	tlasBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	tlasBarrier.UAV.pResource = tlasResultBuffer_.Get();
	dxrCmdList_->ResourceBarrier(1, &tlasBarrier);

// -----------------------------------------------

	// ---------------------------------------------------------
	// 配線図のパラメーターを4つ定義する
	// ---------------------------------------------------------
	CD3DX12_ROOT_PARAMETER rootParameters[5]{};

	// [0] b0: カメラ情報 (Constant Buffer)
	rootParameters[0].InitAsConstantBufferView(0); // レジスタ b0 に直接繋ぐ

	// [1] t0: TLAS (Shader Resource View)
	rootParameters[1].InitAsShaderResourceView(0); // レジスタ t0 に直接繋ぐ

	// [2] t1: 髪の毛の頂点バッファ (Shader Resource View)
	rootParameters[2].InitAsShaderResourceView(1); // レジスタ t1 に直接繋ぐ

	// [3] u0: 出力先のキャンバス (Unordered Access View)
	CD3DX12_DESCRIPTOR_RANGE uavRange;
	uavRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0 を1個
	rootParameters[3].InitAsDescriptorTable(1, &uavRange);

	CD3DX12_DESCRIPTOR_RANGE srvRange;
	srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // t2 を1個
	rootParameters[4].InitAsDescriptorTable(1, &srvRange);

	// ---------------------------------------------------------
	// 配線図を1つにまとめる
	// ---------------------------------------------------------
	CD3DX12_ROOT_SIGNATURE_DESC globalRootSigDesc(ARRAYSIZE(rootParameters), rootParameters);

	// ---------------------------------------------------------
	// シリアライズしてGPU用のRootSignatureオブジェクトを作成
	// ---------------------------------------------------------
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	D3D12SerializeRootSignature(&globalRootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	if (errorBlob) {
		// エラーが出たら中身を出力（OutputDebugStringなど）
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	// メンバ変数の globalRootSignature_ に格納！
	pDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&globalRootSignature_));

	hairShaderBlob_ = CompileRaytracingShader(
		L"resources/shaders/Hair/HairRaytracing.hlsl",
		L"lib_6_5",
		engine->GetDXC().GetUtils().Get(),
		engine->GetDXC().GetCompiler().Get(),
		engine->GetDXC().GetIncludeHandle().Get()
	);

	rtpso_ = CreateHairRaytracingPSO(
		pDevice5.Get(),
		globalRootSignature_.Get(),
		hairShaderBlob_->GetBufferPointer(),
		hairShaderBlob_->GetBufferSize()
	);

// -----------------------------------------------

		// シェーダーバインディングテーブルの作成
	CreateShaderBindingTable(
		rtpso_.Get(),
		pDevice,
		sbtBuffer_,
		dispatchDesc_
	);

	hairCameraBuffer_ = std::make_unique<ConstantBuffer<HairCamera>>(engine_);
	hairCameraBuffer_->Initialize();

	auto initBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		outputTexture_->GetResource(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	dxrCmdList_->ResourceBarrier(1, &initBarrier);
}
//////////////////////////////
//   ====================
// 【      更新処理      】
//   ====================
//////////////////////////////
void Hair::Update(float deltaTime, const Matrix4x4& mat) {
	
	ID3D12DescriptorHeap* ppHeaps[] = { engine_->GetSRV().GetDescriptorHeap().GetHeap().Get() };
	// コマンドリストにヒープをセット（テーブルをセットするより【前】に呼ぶ！）
	dxrCmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	if (isGpuUpdateRequested_) {
		// コピー先に遷移
		auto transitionToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
			gpuGuideBuffer_->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST);

		// バリア開始
		D3D12_RESOURCE_BARRIER barriers[] = { transitionToCopyDest };
		dxrCmdList_->ResourceBarrier(1, barriers);

		// コピー開始
		dxrCmdList_->CopyResource(gpuGuideBuffer_->GetResource(), uploadGuideBuffer->GetResource());

		// コピーが終わったら、UAVバッファを「UAVとして使える状態」に戻す
		auto transitionToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			gpuGuideBuffer_->GetResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		dxrCmdList_->ResourceBarrier(1, &transitionToUAV);

		isGpuUpdateRequested_ = false;
	}

	//   ====================
	// 【 HairCSの初期化処理 】
	//   ====================
	D3D12_RESOURCE_BARRIER preCSBarriers[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(gpuGuideBuffer_->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		CD3DX12_RESOURCE_BARRIER::Transition(hairVertexBuffer_->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		CD3DX12_RESOURCE_BARRIER::Transition(hairAABBBuffer_->GetResource(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	};
	dxrCmdList_->ResourceBarrier(_countof(preCSBarriers), preCSBarriers);

	//   =====================
	// 【 HairGuideの物理演算 】
	//   =====================
	uint32_t totalGuides = gpuConfigBuffer_->GetMappedData()->numGuides;
	uint32_t physicsGroupX = (totalGuides + 63) / 64;

	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairPhysicsCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairPhysicsCS").GetCPS().Get());

	dxrCmdList_->SetComputeRootConstantBufferView(0, gpuPhysicsConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootConstantBufferView(1, gpuFrameConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootDescriptorTable(2, gpuGuideBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(3, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(physicsGroupX, 1, 1);

	auto barrierGuide = CD3DX12_RESOURCE_BARRIER::Transition(
		gpuGuideBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	dxrCmdList_->ResourceBarrier(1, &barrierGuide);

	//   =====================
	// 【 Strands作成の再計算 】
	//   =====================
	uint32_t totalStrands = gpuChildStrandBuffer_->GetNumElements();
	uint32_t generateGroupX = (totalStrands + 63) / 64;

	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairGenerateCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairGenerateCS").GetCPS().Get());
	
	dxrCmdList_->SetComputeRootDescriptorTable(0, gpuGuideBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(1, gpuChildStrandBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(2, hairVertexBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(3, gpuMakeConfigBuffer_->GetGPUVirtualAddress());
	dxrCmdList_->SetComputeRootConstantBufferView(4, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(generateGroupX, 1, 1);

	auto barrierVertices = CD3DX12_RESOURCE_BARRIER::Transition(
		hairVertexBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	dxrCmdList_->ResourceBarrier(1, &barrierVertices);

	//   ==================
	// 【 AABB作成の再計算 】
	//   ==================
	dxrCmdList_->SetComputeRootSignature(PSOManager::GetInstance()->GetPSO("HairAABBCS").GetRootSignature().GetRS().Get());
	dxrCmdList_->SetPipelineState(PSOManager::GetInstance()->GetPSO("HairAABBCS").GetCPS().Get());
	// ここにRootSignatureのやつを設定していく
	dxrCmdList_->SetComputeRootDescriptorTable(0, hairVertexBuffer_->GetSRVHandleGPU());
	dxrCmdList_->SetComputeRootDescriptorTable(1, hairAABBBuffer_->GetUAVHandleGPU());
	dxrCmdList_->SetComputeRootConstantBufferView(2, gpuConfigBuffer_->GetGPUVirtualAddress());

	dxrCmdList_->Dispatch(generateGroupX, 1, 1);

	D3D12_RESOURCE_BARRIER uavBarrier = {};
	uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	uavBarrier.UAV.pResource = hairAABBBuffer_->GetResource(); // AABBバッファ
	dxrCmdList_->ResourceBarrier(1, &uavBarrier);

	auto barrierAABB = CD3DX12_RESOURCE_BARRIER::Transition(
		hairAABBBuffer_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	dxrCmdList_->ResourceBarrier(1, &barrierAABB);

	//   ==============
	// 【 BLASの再構築 】
	//   ==============
	// ジオメトリ（AABBバッファ）の設定を最新の状態に更新
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc{};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
	geometryDesc.AABBs.AABBCount = hairAABBBuffer_->GetNumElements();
	geometryDesc.AABBs.AABBs.StartAddress = hairAABBBuffer_->GetResource()->GetGPUVirtualAddress();
	geometryDesc.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);

	// ビルド用のインプットを再設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInputs{};
	blasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	blasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE
					 | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
					 | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
	blasInputs.NumDescs = 1;
	blasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	blasInputs.pGeometryDescs = &geometryDesc;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC blasBuildDesc{};
	blasBuildDesc.Inputs = blasInputs;
	// アップデートなので、結果を既存のBLASバッファに上書きする
	blasBuildDesc.DestAccelerationStructureData = blasResultBuffer_->GetGPUVirtualAddress();
	blasBuildDesc.SourceAccelerationStructureData = blasResultBuffer_->GetGPUVirtualAddress();
	//blasBuildDesc.SourceAccelerationStructureData = 0;
	blasBuildDesc.ScratchAccelerationStructureData = scratchBuffer_->GetGPUVirtualAddress();

	// GPUに向けて、BLASのアップデートコマンドを発行！
	dxrCmdList_->BuildRaytracingAccelerationStructure(&blasBuildDesc, 0, nullptr);

	// BLASの書き込み完了を待つUAVバリア
	D3D12_RESOURCE_BARRIER blasBarrier{};
	blasBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	blasBarrier.UAV.pResource = blasResultBuffer_.Get();
	dxrCmdList_->ResourceBarrier(1, &blasBarrier);

// -----------------------------------------------
	//   ================
	// 【 TLASの更新処理 】
	//   ================
// -----------------------------------------------

	instanceBuffer->GetMappedData()->Transform[0][0] = mat.m[0][0];
	instanceBuffer->GetMappedData()->Transform[0][1] = mat.m[1][0];
	instanceBuffer->GetMappedData()->Transform[0][2] = mat.m[2][0];
	instanceBuffer->GetMappedData()->Transform[0][3] = mat.m[3][0];

	instanceBuffer->GetMappedData()->Transform[1][0] = mat.m[0][1];
	instanceBuffer->GetMappedData()->Transform[1][1] = mat.m[1][1];
	instanceBuffer->GetMappedData()->Transform[1][2] = mat.m[2][1];
	instanceBuffer->GetMappedData()->Transform[1][3] = mat.m[3][1] + 1.42f;

	instanceBuffer->GetMappedData()->Transform[2][0] = mat.m[0][2];
	instanceBuffer->GetMappedData()->Transform[2][1] = mat.m[1][2];
	instanceBuffer->GetMappedData()->Transform[2][2] = mat.m[2][2];
	instanceBuffer->GetMappedData()->Transform[2][3] = mat.m[3][2];

// -----------------------------------------------

	// Inputsの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInputs{};
	tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	tlasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE
					 | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE
					 /*| D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE*/;
	tlasInputs.NumDescs = 1; // 配置するインスタンス数
	tlasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	tlasInputs.InstanceDescs = instanceBuffer->GetResource()->GetGPUVirtualAddress();

	// BuildDescの設定
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlasBuildDesc{};
	tlasBuildDesc.Inputs = tlasInputs;
	tlasBuildDesc.ScratchAccelerationStructureData = tlasScratchBuffer_->GetGPUVirtualAddress();
	tlasBuildDesc.DestAccelerationStructureData = tlasResultBuffer_->GetGPUVirtualAddress();
	//tlasBuildDesc.SourceAccelerationStructureData = tlasResultBuffer_->GetGPUVirtualAddress();
	tlasBuildDesc.SourceAccelerationStructureData = 0;

	// GPUにビルドし直せコマンドを積む
	dxrCmdList_->BuildRaytracingAccelerationStructure(&tlasBuildDesc, 0, nullptr);

	// TLASのビルドが終わるまで、この先のレイ飛ばし（DispatchRays）を待たせるバリア
	D3D12_RESOURCE_BARRIER tlasBarrier{};
	tlasBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	tlasBarrier.UAV.pResource = tlasResultBuffer_.Get();
	dxrCmdList_->ResourceBarrier(1, &tlasBarrier);

// -----------------------------------------------
#ifdef USE_IMGUI
	ImGui::Begin("Hair");
	ImGui::DragFloat3("gravity", &gpuFrameConfigBuffer_->GetMappedData()->gravity.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat3("Wind Direction ", &gpuFrameConfigBuffer_->GetMappedData()->windDirection.x, 0.01f, -1.0f, 1.0f);

	ImGui::DragFloat("globalHairThickness ", &gpuMakeConfigBuffer_->GetMappedData()->globalHairThickness, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("stiffness：剛性 ", &gpuPhysicsConfigBuffer_->GetMappedData()->stiffness, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("restoringForce：復元力", &gpuPhysicsConfigBuffer_->GetMappedData()->restoringForce, 0.0001f, 0.0f, 1.0f);
	ImGui::DragFloat("damping：減衰力", &gpuPhysicsConfigBuffer_->GetMappedData()->damping, 0.0001f, 0.0f, 1.0f);
	ImGui::End();
#endif// USE_IMGUI
}

void Hair::Render(D3D12_GPU_DESCRIPTOR_HANDLE depthHandle) {
	// Get Camera Data
	HairCamera cameraData;
	cameraData.inverseProjView = Matrix4x4::Inverse(CameraSystem::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix());
	cameraData.cameraPosition = CameraSystem::GetInstance()->GetActiveCamera()->GetTranslation();
	cameraData.padding = 0.0f; // パディングを0で初期化
	hairCameraBuffer_->GetMappedData()[0] = cameraData;

	auto commandList = engine_->GetCommand().GetList().GetList().Get();

	// --- グローバルルートシグネチャの設定 ---
	// レイトレ全体で使う共通のバッファ（TLASや出力テクスチャなど）をシェーダーに紐付け
	commandList->SetComputeRootSignature(globalRootSignature_.Get());

	commandList->SetComputeRootConstantBufferView(0, hairCameraBuffer_->GetGPUVirtualAddress());

	commandList->SetComputeRootShaderResourceView(1, tlasResultBuffer_->GetGPUVirtualAddress());

	commandList->SetComputeRootShaderResourceView(2, hairVertexBuffer_->GetResource()->GetGPUVirtualAddress());

	commandList->SetComputeRootDescriptorTable(3, renderTargetUAVHandleGPU_);

	commandList->SetComputeRootDescriptorTable(4, depthHandle);

	// --- 2. パイプライン（RTPSO）のセット ---
	// コマンドリストにレイトレ専用パイプラインを設定（StateObjectをセットします）
	dxrCmdList_->SetPipelineState1(rtpso_.Get());

	// --- 3. DXRレイトレーシングの実行！ ---
	// 前ステップで SBT の情報を詰め込んだ `dispatchDesc` に、画面サイズを設定します
	UINT windowWidth = 1280;  // 画面の横幅（例: 1920）
	UINT windowHeight = 720; // 画面の縦幅（例: 1080）
	dispatchDesc_.Width = windowWidth;  // 画面の横幅（例: 1920）
	dispatchDesc_.Height = windowHeight; // 画面の縦幅（例: 1080）
	dispatchDesc_.Depth = 1;            // 2D画面なので1でOK

	// GPUにレイトレーシングの開始を命令！
	dxrCmdList_->DispatchRays(&dispatchDesc_);
}

//////////////////////////////////////
///
///  ココから下にあるやつは描画したあとの処理
/// 
//////////////////////////////////////

void Hair::PreHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES preState, ID3D12Resource* depthResource) {
	auto commandList = engine_->GetCommand().GetList().GetList().Get();

	// 今描画されている画像をコピーする
	auto transitionTargetSrc = CD3DX12_RESOURCE_BARRIER::Transition(
		randerTargetResource,
		preState,
		D3D12_RESOURCE_STATE_COPY_SOURCE);
	// コピー先
	auto transitionUAVDest = CD3DX12_RESOURCE_BARRIER::Transition(
		outputTexture_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST);

	auto transitionDepth = CD3DX12_RESOURCE_BARRIER::Transition(
		depthResource,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,                  // 元の状態
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE      // レイトレ(Compute)で読める状態
	);

	D3D12_RESOURCE_BARRIER barriers1[] = { transitionTargetSrc, transitionUAVDest, transitionDepth };
	commandList->ResourceBarrier(_countof(barriers1), barriers1);

	// それまでに描画されていた絵（青色など）をUAVテクスチャへ丸ごとコピー！
	commandList->CopyResource(outputTexture_->GetResource(), randerTargetResource);

// -------------------------------------------------------

	// UAV（レイトレ結果）を「書き込み用(UAV)」から「コピー元(COPY_SOURCE)」へ変更
	auto transitionToCopySrc = CD3DX12_RESOURCE_BARRIER::Transition(
		outputTexture_->GetResource(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// バックバッファ（画面）を「表示用(PRESENT)」などから「コピー先(COPY_DEST)」へ変更
	auto transitionToCopyDest = CD3DX12_RESOURCE_BARRIER::Transition(
		randerTargetResource,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_RESOURCE_BARRIER barriers[] = { transitionToCopySrc, transitionToCopyDest };
	commandList->ResourceBarrier(_countof(barriers), barriers);
}

void Hair::PostHair(ID3D12Resource* randerTargetResource, D3D12_RESOURCE_STATES postState, ID3D12Resource* depthResource) {
	auto commandList = engine_->GetCommand().GetList().GetList().Get();

	auto transitionToCopySrc = CD3DX12_RESOURCE_BARRIER::Transition(
		outputTexture_->GetResource(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(1, &transitionToCopySrc);

	// UAVの絵を、バックバッファに全コピー！
	commandList->CopyResource(randerTargetResource, outputTexture_->GetResource());

	// バックバッファを「コピー先」から「表示用(PRESENT)」に戻す
	auto transitionToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		randerTargetResource,
		D3D12_RESOURCE_STATE_COPY_DEST,
		postState);
	commandList->ResourceBarrier(1, &transitionToPresent);

	auto transitionToUAV = CD3DX12_RESOURCE_BARRIER::Transition(
		outputTexture_->GetResource(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandList->ResourceBarrier(1, &transitionToUAV);

	auto depthTransition = CD3DX12_RESOURCE_BARRIER::Transition(
		depthResource,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
	commandList->ResourceBarrier(1, &depthTransition);
}