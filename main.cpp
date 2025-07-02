
#include <dxgidebug.h>//0103 ReportLiveobject
#include "Window.h"
#include "ErrorGuardian.h"
#include "Log.h"
#include "DXGI.h"
#include "D3D12System.h"
#include "TheOrderCommand.h"
#include "SwapChain.h"
#include "TachyonSync.h"
#include "OmnisTechOracle.h"
#include "RenderTargetView.h"
#include "Texture.h"
#include "PipelineStateObject.h"
#include "OffScreenRendering.h"

#include "SpriteObject.h"
#include "ModelObject.h"
#include "TriangleObject.h"
#include "SphereObject.h"

#include "DirectionLight.h"

#include "ImGuiManager.h"
#include "Structures.h"
#include "ResourceBarrier.h"
#include "InputManager.h"
#include "Mouse.h"
#include "Music.h"
#include "CameraBase.h"
#include "DebugCamera.h"
#include "Chronos.h"

#include "Player.h"
#include "Enemy.h"
#include "SkyDome.h"
#include "MapChip.h"
#include "CameraController.h"
#include "AABB.h"
#include "DeathParticle.h"


#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")//0103 ReportLiveobject


#define pi float(3.14159265358979323846f)

struct D3D12ResourceLeakChecker {
	~D3D12ResourceLeakChecker() {
		// リソースリーク✅
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		//リソースリークチェック==================-↓↓↓
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			//debug->Release();
		}
		//リソースリークチェック==================-↑↑↑
	}
};

// windowsアプリでのエントリーポイント（main関数）
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3D12ResourceLeakChecker leakCheck;
	int32_t kClienWidth = 1280;
	int32_t kClienHeight = 720;
	InputManager input;
#pragma region Engine
////////////////////////////////////////////////////////////
#pragma region DirectX12の初期化独立している

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//誰も補足しなかった場合に（Unhandled）、補足する関数を登録
	//main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ErrorGuardian::ExportDump);

	Log::Initialize();

#pragma endregion
////////////////////////////////////////////////////////////

#pragma region ウィンドウ
#pragma endregion
	Window window;
	//ウィンドウの生成
	window.Initialize(L"CG2CLASSNAME", L"CG2");
#pragma region デバッグレイヤー(開発時のデバッグ支援)
#pragma endregion
	ErrorGuardian::SetDebugInterface();
#pragma region DXGIファクトリーの作成
#pragma endregion
	DXGI::RecruitEngineer();
	HRESULT hr;
#pragma region 使用するアダプタの決定
#pragma endregion
	OmnisTechOracle::Oracle();
#pragma region D3D12Deviceの生成
#pragma endregion
	D3D12System d3d12;
	d3d12.SelectLevel();
#pragma region エラー警告 直ちに停止独立している
#pragma endregion
	ErrorGuardian::SetQueue(d3d12.GetDevice().Get());
#pragma region CommandQueueの生成
#pragma endregion
#pragma region CommandList
#pragma endregion
	TheOrderCommand command;
	command.GetQueue().SetDescD();
	hr = d3d12.GetDevice()->CreateCommandQueue(&command.GetQueue().GetDesc(), IID_PPV_ARGS(&command.GetQueue().GetQueue()));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	hr = d3d12.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	IID_PPV_ARGS(&command.GetList().GetAllocator()));
	//コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));
	hr = d3d12.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command.GetList().GetAllocator().Get(), nullptr,
	IID_PPV_ARGS(&command.GetList().GetList()));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
#pragma region SwapChain
#pragma endregion
	SwapChain::Initialize(window);
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	DXGI::AssignTaskToEngineer(command.GetQueue().GetQueue().Get(), window);
#pragma region SwapChainからResourceを引っ張ってくる
#pragma endregion
	SwapChain::MakeResource();
#pragma region DescriptorSize
#pragma endregion
	SRV::InitializeHeap(d3d12); 
#pragma region RTVを作る
#pragma endregion
	RenderTargetView rtv;
	rtv.Initialize(&d3d12);
#pragma region ディスクリプタヒープの生成
#pragma endregion
	DepthStencil dsv;
	dsv.InitializeHeap(d3d12);
#pragma region DepthStencilTexture
#pragma endregion
	dsv.MakeResource(d3d12, kClienWidth, kClienHeight);
#pragma region DepthStencilView(DSV)
#pragma endregion
	//DSVHeapの先頭にDSVを作る
	d3d12.GetDevice()->CreateDepthStencilView(dsv.GetResource().Get(), &dsv.GetDSVDesc(), dsv.GetHeap().GetHeap()->GetCPUDescriptorHandleForHeapStart());
#pragma region FenceとEvent
#pragma endregion
	TachyonSync::GetCGPU().Initialize(d3d12.GetDevice().Get());
#pragma region DXCの初期化
#pragma endregion
#pragma region Pipeline State Object
#pragma endregion
#pragma region PSOを生成
#pragma region DepthStencilStateの設定
#pragma endregion
#pragma endregion
	PSO pso;
	pso.Initialize(d3d12, PSOTYPE::Normal);

	OffScreenRendering osr;
	osr.Initialize(d3d12,1280.0f,720.0f);
#pragma region ViewportとScissor独立しているが後回し
	//ビューポート
	D3D12_VIEWPORT viewport = {};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(window.GetWindowRect().right);
	viewport.Height = static_cast<float>(window.GetWindowRect().bottom);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//シザー矩形
	D3D12_RECT scissorRect = {};
	//個本的にビューポート同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = window.GetWindowRect().right;
	scissorRect.top = 0;
	scissorRect.bottom = window.GetWindowRect().bottom;
#pragma endregion
	ImGuiManager::SetImGui(window.GetHwnd(), d3d12.GetDevice().Get(), SRV::descriptorHeap_.GetHeap().Get());
#pragma region XAudio2初期化
#pragma endregion
	Music music;
	music.Initialize();
#pragma region 入力情報の初期化
#pragma endregion
	input.Initialize(window.GetWindowClass(), window.GetHwnd());

	DirectionLight light;
	light.Initialize(d3d12);
#pragma endregion

	ModelObject model;
	model.Initialize(d3d12, "axis.obj");

	Texture tex;
	tex.Initialize(d3d12, model.GetFilePath(),1);

	//   ここまでモデル系の処理

	MSG msg{};
	DebugCamera debugCamera;
	debugCamera.Initialize();
	CameraBase cameraBase;
	cameraBase.Initialize();
	//Audio audio;
	//audio.SoundPlayWave(MediaAudioDecoder::DecodeAudioFile(L"resources/maou_bgm_fantasy02.mp3"));
	music.GetBGM().LoadWAVE("resources/loop101204.wav");
	//music.GetBGM().SetPlayAudioBuf();
	 
	Transform transform;
	transform = {
		{ 1.0f, 1.0f, 1.0f }, // scale
		{ 0.0f, 0.0f, 0.0f }, // rotate
		{ 0.0f, 0.0f, 0.0f } // translate
	};
	//cameraBase.SetTargetPos(player.GetWorldTransform().translate);
	//ウィンドウのｘボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		
		//Widnowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ImGuiManager::BeginFrame();
			Chronos::Update();
			input.Update();
			debugCamera.UpData();
			
			cameraBase.UpDate();
#pragma region OffScreenRendering
			/*ResourceBarrier barrierO = {};
			barrierO.SetBarrier(command.GetList().GetList().Get(), osr.GetResource().Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
			osr.Begin(command);*/
			//ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::descriptorHeap_.GetHeap().Get() };
			//command.GetList().GetList()->SetDescriptorHeaps(1, descriptorHeaps);
			///////////////////////////////////////////////////////////////////////////
			////描画0200
			//command.GetList().GetList()->RSSetViewports(1, &viewport);
			//command.GetList().GetList()->RSSetScissorRects(1, &scissorRect);

			

			//model.SetWVPData(debugCamera.DrawMirrorCamera(worldMatrix, transformSprite.translate, {0.0f,0.0f,-1.0f}), worldMatrix, uvTransformMatrix);

			//model.Draw(command, pso, light, tex);

			//barrierO.SetTransition(command.GetList().GetList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
#pragma endregion
////////////////////////////////////////////////////////////
#pragma region コマンドを積み込んで確定させる
	//これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = SwapChain::swapChain_->GetCurrentBackBufferIndex();

			ResourceBarrier barrier = {};
			barrier.SetBarrier(command.GetList().GetList().Get(), SwapChain::GetResource(backBufferIndex).Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			//描画先のRTVを設定する
			//command.GetList().GetList()->OMSetRenderTargets(1, &rtv.GetHandle(backBufferIndex), false, nullptr);
			//描画先のRTVとDSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv.GetHeap().GetHeap()->GetCPUDescriptorHandleForHeapStart();
			command.GetList().GetList()->OMSetRenderTargets(1, &rtv.GetHandle(backBufferIndex), false, &dsvHandle);
			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//RGBAの設定
			command.GetList().GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);//
			command.GetList().GetList()->ClearRenderTargetView(rtv.GetHandle(backBufferIndex), clearColor, 0, nullptr);

			ID3D12DescriptorHeap* descriptorHeaps[] = { SRV::descriptorHeap_.GetHeap().Get() };
			command.GetList().GetList()->SetDescriptorHeaps(1, descriptorHeaps);
			/////////////////////////////////////////////////////////////////////////
			//描画0200
			command.GetList().GetList()->RSSetViewports(1, &viewport);
			command.GetList().GetList()->RSSetScissorRects(1, &scissorRect);
			
#pragma endregion
////////////////////////////////////////////////////////////
		

			//   ここからゲームの処理↓↓↓↓
		
			Matrix4x4 world = Matrix4x4::Make::Affine(
				transform.scale,
				transform.rotate,
				transform.translate
			);

			//   ここまでゲームの処理↑↑↑↑
			//   ここから描画関係処理↓↓↓↓
			model.SetWVPData(cameraBase.DrawCamera(world), world, world);
			model.Draw(command, pso, light, tex);

			//   ここまで描画関係処理↑↑↑↑
			//描画
			ImGuiManager::EndFrame(command.GetList().GetList());
			//barrierO.SetTransition(command.GetList().GetList().Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
			barrier.SetTransition(command.GetList().GetList().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			//画面表示できるようにするために
			//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
			hr = command.GetList().GetList()->Close();
			assert(SUCCEEDED(hr));

////////////////////////////////////////////////////////////
#pragma region 積んだコマンドをキックする
	//GPUにコマンドリストの実行を行わさせる
			ID3D12CommandList* commandLists[] = { command.GetList().GetList().Get()};
			command.GetQueue().GetQueue()->ExecuteCommandLists(1, commandLists);


			//GPUとOSに画面の交換を行うように通知する
			SwapChain::swapChain_->Present(1, 0);

			TachyonSync::GetCGPU().Update(command.GetQueue().GetQueue().Get());


			//次のフレーム用のコマンドリストを準備
			hr = command.GetList().GetAllocator()->Reset();
			assert(SUCCEEDED(hr));
			hr = command.GetList().GetList()->Reset(command.GetList().GetAllocator().Get(), nullptr);
			assert(SUCCEEDED(hr));
#pragma endregion
////////////////////////////////////////////////////////////
		}
		
	}
	ImGuiManager::Shutdown();

	//解放処理
	TachyonSync::GetCGPU().UnLoad();
	music.UnLoad();


#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG
	CloseWindow(window.GetHwnd());

	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
