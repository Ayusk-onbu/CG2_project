
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
#include <algorithm>

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

	//   ここからモデル系の処理
	ModelObject playerModel;
	playerModel.Initialize(d3d12, "player.obj");
	Texture playerTex;
	playerTex.Initialize(d3d12, playerModel.GetFilePath(), 1);
	ModelObject ATKEffectModel;
	ATKEffectModel.Initialize(d3d12, "attackEffect.obj");

	ModelObject skyDomeModel;
	skyDomeModel.Initialize(d3d12, "skyDome.obj");
	Texture skyDomeTex;
	skyDomeTex.Initialize(d3d12, skyDomeModel.GetFilePath(), 2);

	ModelObject mapChipModel[20][100];
	for (int i = 0;i < 20;++i) {
		for (int j = 0;j < 100;++j) {
			mapChipModel[i][j].Initialize(d3d12, "brickBlock.obj");
		}
	}
	Texture mapChipTex;
	mapChipTex.Initialize(d3d12, mapChipModel[0][0].GetFilePath(), 3);

	ModelObject deathParticleModel[8];
	for (int i = 0;i < 8;++i) {
		deathParticleModel[i].Initialize(d3d12, "deathParticle.obj");
	}

	Texture deathParticleTex;
	deathParticleTex.Initialize(d3d12, deathParticleModel[0].GetFilePath(), 4);

	std::vector<ModelObject*> enemyModel;
	for (int i = 0;i < 3;++i) {
		ModelObject* newModel = new ModelObject;
		newModel->Initialize(d3d12, "player.obj");
		enemyModel.push_back(newModel);
	}

	SpriteObject fadeModel;
	fadeModel.Initialize(d3d12, 1280.0f, 720.0f);

	//   ここまでモデル系の処理

	MSG msg{};
	CameraBase cameraBase;
	cameraBase.Initialize();
	//Audio audio;
	//audio.SoundPlayWave(MediaAudioDecoder::DecodeAudioFile(L"resources/maou_bgm_fantasy02.mp3"));
	music.GetBGM().LoadWAVE("resources/loop101204.wav");
	//music.GetBGM().SetPlayAudioBuf();
	 
	SkyDome skyDome;
	skyDome.Initialize(skyDomeModel, cameraBase);
	MapChip mapChip;
	mapChip.LoadMapChipCsv("resources/mapChip.csv");
	std::vector<std::vector<Transform*>> worldTransformBlocks_;
	std::vector<std::vector<Transform*>> uvTransformBlocks_;
	Transform uvTransformBlock= { {1.0f, 1.0f, 1.0f},
					              {0.0f, 0.0f, 0.0f},
					              {0.0f, 0.0f, 0.0f} };
	//要素数
	uint32_t kNumBlockVirtical = mapChip.GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChip.GetNumBlockHorizontal();
	//要素数を変更
	//列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}
	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChip.GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				Transform* worldTransform = new Transform;
				worldTransform->scale = { 1.0f,1.0f,1.0f };
				worldTransform->rotate = { 0.0f,0.0f,0.0f };
				worldTransform->translate = {0.0f,0.0f,0.0f};
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translate = mapChip.GetBlockPositionByIndex(j, i);
			}
		}
	}
	Player player;
	//player.Initialize(playerModel, cameraBase, Vector3 (0.0f,0.0f,0.0f));
	player.Initialize(playerModel, cameraBase,mapChip.GetBlockPositionByIndex(2,18));
	player.EffectInitialize(ATKEffectModel);
	player.SetMapChipField(&mapChip);

	std::list<Enemy*> enemys;
	for (int i = 0;i < 3;++i) {
		Enemy* newEnemy = new Enemy;
		newEnemy->Initialize(enemyModel[i], &cameraBase, mapChip.GetBlockPositionByIndex(10 + i * 2, 18));
		enemys.push_back(newEnemy);
	}
	DeathParticle deathParticle;
	deathParticle.Initialize(player.GetWorldTransform().translate);

	CameraController cameraController;
	cameraController.Initialize(&cameraBase);
	cameraController.SetTarget(&player);
	cameraController.Reset();
	//カメラの移動範囲を設定
	Rect area = { 0, 100, 0, 100 };
	area.left = 11;
	area.right = mapChip.GetBlockPositionByIndex(100, 0).x;
	area.bottom = mapChip.GetBlockPositionByIndex(0, 13).y;
	area.top = mapChip.GetBlockPositionByIndex(0, 20).y - 10;
	cameraController.SetMovableArea(area);
	cameraBase.SetTargetPos(player.GetWorldTransform().translate);

	enum SCENE {
		Title,
		Game,
	};
	SCENE scene = Title;
	enum FADE {
		In,
		None,
		Out,
	};
	Vector4 fadeColor = { 0.0f,0.0f,0.0f,1.0f };
	float fadeTimer = 0.0f;
	float fadeLimit = 3.0f;
	bool isFade = false;
	uint32_t fadeType = In;

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
		
			
			cameraController.Update();
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
		
			//fadeTimer = std::clamp(fadeTimer, 0.0f, fadeLimit);
			switch (scene) {
			case Title:
				switch (fadeType) {
				case In:
					if (fadeLimit <= fadeTimer) {
						fadeType = None;
						fadeTimer = 0.0f;
					}
					fadeTimer += 1.0f / 60.0f;
					fadeColor.w = 1.0f * (fadeLimit - fadeTimer) / fadeLimit;
					break;
				case None:
					if (InputManager::GetKey().PressKey(DIK_SPACE)) {
						fadeType = Out;
					}
					fadeColor.w = 0.0f;
					break;
				case Out:
					if (fadeLimit <= fadeTimer) {
						fadeType = In;
						scene = Game;
						fadeTimer = 0.0f;
					}
					fadeTimer += 1.0f / 60.0f;
					fadeColor.w = 1.0f * fadeTimer / fadeLimit;
					break;
				}
				

				
				break;
			case Game:
				switch (fadeType) {
				case In:
					if (fadeLimit <= fadeTimer) {
						fadeType = None;
						fadeTimer = 0.0f;
					}
					fadeTimer += 1.0f / 60.0f;
					fadeColor.w = 1.0f * (fadeLimit - fadeTimer) / fadeLimit;
					break;
				case None:
					if (player.IsDead()) {
						if (deathParticle.IsFinished()) {
							fadeType = Out;
						}
					}
					fadeColor.w = 0.0f;
					break;
				case Out:
					if (fadeLimit <= fadeTimer) {
						fadeType = In;
						scene = Title;
						fadeTimer = 0.0f;
					}
					fadeTimer += 1.0f / 60.0f;
					fadeColor.w = 1.0f * fadeTimer / fadeLimit;
					break;
				}

				player.Update();
				for (Enemy* enemy : enemys) {
					enemy->UpDate();
				}
				skyDome.Update();
				// ブロックの更新
				for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
					for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
						if (mapChip.GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
							// uv
							Matrix4x4 uvBlock = Matrix4x4::Make::Scale(uvTransformBlock.scale);
							uvBlock = Matrix4x4::Multiply(uvBlock, Matrix4x4::Make::RotateZ(uvTransformBlock.rotate.z));
							uvBlock = Matrix4x4::Multiply(uvBlock, Matrix4x4::Make::Translate(uvTransformBlock.translate));

							Matrix4x4 matWorld;
							matWorld = Matrix4x4::Make::Affine(worldTransformBlocks_[i][j]->scale, worldTransformBlocks_[i][j]->rotate, worldTransformBlocks_[i][j]->translate);
							mapChipModel[i][j].SetWVPData(cameraBase.DrawCamera(matWorld), matWorld, uvBlock);
						}
					}
				}

				AABB aabb1, aabb2;
				aabb1 = AABB::World2AABB(player.GetWorldTransform().translate);

				for (Enemy* enemy : enemys) {
					aabb2 = AABB::World2AABB(enemy->GetWorldTransform().translate);

					if (AABB::IsHitAABB2AABB(aabb1, aabb2)) {
						
						if (!player.IsDead()) {
							deathParticle.SetPosition(player.GetWorldTransform().translate);
						}
						player.OnCollision(enemy);
						enemy->OnCollision(&player);
					}

				}
				if (!deathParticle.IsFinished()) {
					deathParticle.UpDate();
				}
				ImGui::Begin("DirectionalLight");
				ImGuiManager::CreateImGui("Direction", light.directionalLightData_->direction, -1.0f, 1.0f);
				ImGui::End();
				break;
			}
			Matrix4x4 world = Matrix4x4::Make::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,Deg2Rad(180),0.0f }, { 640.0f,-360.0f,0.0f });
			fadeModel.SetWVPData(world, world, world);
			fadeModel.SetColor(fadeColor);
			
			switch (scene) {
			case Title:
				
				break;
			case Game:
				//   ここまでゲームの処理↑↑↑↑
			//   ここから描画関係処理↓↓↓↓
				player.Draw(command, pso, light, playerTex);
				for (Enemy* enemy : enemys) {
					enemy->Draw(command, pso, light, playerTex);
				}
				skyDome.Draw(command, pso, light, skyDomeTex);
				for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
					for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
						if (mapChip.GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
							mapChipModel[i][j].Draw(command, pso, light, mapChipTex);
						}
					}
				}
				if (player.IsDead()) {
					deathParticle.SetPosition(player.GetWorldTransform().translate);
					if (!deathParticle.IsFinished()) {
						for (int i = 0;i < 8;++i) {
							deathParticleModel[i].SetWVPData(cameraBase.DrawCamera(deathParticle.GetPosition(i)), deathParticle.GetPosition(i), deathParticle.GetPosition(i));
							deathParticleModel[i].SetColor(deathParticle.GetColor());
							deathParticleModel[i].Draw(command, pso, light, deathParticleTex);
						}
					}
				}
				break;
			}
			fadeModel.Draw(command, pso, light, deathParticleTex);
			
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
	for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Transform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	for (std::vector<Transform*>& worldTransformBlockLine : uvTransformBlocks_) {
		for (Transform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	uvTransformBlocks_.clear();
	for (ModelObject* enemy : enemyModel) {
		delete enemy;
	}
	enemyModel.clear();
	for (Enemy* enemy : enemys) {
		delete enemy;
	}
	enemys.clear();

#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG
	CloseWindow(window.GetHwnd());

	//COMの初期化を解除
	CoUninitialize();
	return 0;
}
