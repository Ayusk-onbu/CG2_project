#include "PipelineStateObject.h"
#include <cassert>


void PipelineStateObject::Initialize(D3D12System& d3d12, PSOTYPE type) {
	InitializeDescs(d3d12, type);
	MargeDesc();
	depthStencil_.InitializeDesc();
	SetDesc(d3d12,type);
}

void PipelineStateObject::InitializeDescs(D3D12System& d3d12, PSOTYPE type) {
	if (type == PSOTYPE::Normal) {
		
	}
	dxc_.Initialize();
	rootSignature_.CreateRootSignature(d3d12, ROOTTYPE::Normal);
	inputLayoutDesc_.Initialize();
	blendState_.Initialize(USECOLOR::All);
	rasterizer_.SetDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	if (type == PSOTYPE::Line) {
		rasterizer_.GetDesc().AntialiasedLineEnable = true;
	}
	Compile();
}

void PipelineStateObject::Compile() {
	vertexShaderBlob_ = CompileShader(L"resources/shaders/Object3D.VS.hlsl",
		L"vs_6_0", dxc_.GetUtils().Get(), dxc_.GetCompiler().Get(), dxc_.GetIncludeHandle().Get());
	assert(vertexShaderBlob_ != nullptr);
	pixelShaderBlob_ = CompileShader(L"resources/shaders/Object3D.PS.hlsl",
		L"ps_6_0", dxc_.GetUtils().Get(), dxc_.GetCompiler().Get(), dxc_.GetIncludeHandle().Get());
	assert(pixelShaderBlob_ != nullptr);
}

//CompileShader関数
Microsoft::WRL::ComPtr < IDxcBlob> PipelineStateObject::CompileShader(
	//CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したものを三つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
{
	// ここの中身をこの後書いていく
	// １，hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	Log::View(ConvertString(std::format(L"Begin CompileShader,path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	// ２，Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// ３，警告・エラーが惰ていないか確認する
	//警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log::View(shaderError->GetStringPointer());
		//警告エラーダメ絶対
		assert(false);
	}
	// ４，Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr < IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log::View(ConvertString(std::format(L"CompileSucceeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

void PipelineStateObject::MargeDesc() {
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.GetRS().Get();//RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_.GetDesc();//InputLayout
	graphicsPipelineStateDesc_.RasterizerState = rasterizer_.GetDesc();//RasterizerState
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };// VertexShader
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };//PixelShader
}

void PipelineStateObject::SetDesc(D3D12System& d3d12, PSOTYPE type) {
	//書きこむRYVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトボロジ（形状）のタイプ。三角形
	if (type == PSOTYPE::Line) {
		graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	else {
		graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencil
	graphicsPipelineStateDesc_.DepthStencilState = depthStencil_.GetDesc();
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// ---  --------------- ------------ ------ //
	//  BlendState                              //
	//  -------- -------- -------  -- --------- //
	blendState_.SetBlendMode(BLENDMODE::AlphaBlend);//BlendStateの設定
	graphicsPipelineStateDesc_.BlendState = blendState_.GetDesc();//BlendState

	//実際に生成
	HRESULT hr;
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));

	blendState_.SetBlendMode(BLENDMODE::Additive);//BlendStateの設定
	graphicsPipelineStateDesc_.BlendState = blendState_.GetDesc();//BlendState

	//実際に生成
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_Add));
	assert(SUCCEEDED(hr));

	blendState_.SetBlendMode(BLENDMODE::Subtractive);//BlendStateの設定
	graphicsPipelineStateDesc_.BlendState = blendState_.GetDesc();//BlendState

	//実際に生成
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_Sub));
	assert(SUCCEEDED(hr));

	blendState_.SetBlendMode(BLENDMODE::ScreenBlend);//BlendStateの設定
	graphicsPipelineStateDesc_.BlendState = blendState_.GetDesc();//BlendState

	//実際に生成
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_Scr));
	assert(SUCCEEDED(hr));

	blendState_.SetBlendMode(BLENDMODE::Multiplicative);//BlendStateの設定
	graphicsPipelineStateDesc_.BlendState = blendState_.GetDesc();//BlendState

	//実際に生成
	hr = d3d12.GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_Mul));
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr <ID3D12PipelineState>& PipelineStateObject::GetGPS() {
	if (blendMode_ == BLENDMODE::AlphaBlend) {
		return graphicsPipelineState_;
	}
	else if (blendMode_ == BLENDMODE::Additive) {
		return graphicsPipelineState_Add;
	}
	else if (blendMode_ == BLENDMODE::Subtractive) {
		return graphicsPipelineState_Sub;
	}
	else if (blendMode_ == BLENDMODE::Multiplicative) {
		return graphicsPipelineState_Mul;
	}
	else if (blendMode_ == BLENDMODE::ScreenBlend) {
		return graphicsPipelineState_Scr;
	}
	else {
		return graphicsPipelineState_;
	}
}
