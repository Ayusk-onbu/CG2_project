
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
#include "LineObject.h"

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
#include "RandomUtils.h"
#include "SceneDirector.h"
#include "MathUtils.h"

//GameStart

#include "Player.h"
#include "Enemy.h"
#include "CollisionManager.h"
#include "SkyDome.h"
#include "Ground.h"
#include "RailCameraController.h"
#include "LockOn.h"

//GameEnd

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
//
//void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

void PopEnemy(std::list<Enemy*>& enemies_, D3D12System& d3d12, CameraBase* camera, ModelObject& enemyModel, Texture& enemyTex, ModelObject& enemyBulletModel, Texture& enemyBulletTex,std::list<EnemyBullet*>& enemyBullets_, const Vector3& pos,const Player&player, uint32_t movePattern);

void LoadEnemyPopData(std::stringstream& command, std::string filepath);

void UpdateEnemyPopCommand(std::stringstream& command, std::list<Enemy*>& enemies_, D3D12System& d3d12, CameraBase* camera, ModelObject& enemyModel, Texture& enemyTex, ModelObject& enemyBulletModel, Texture& enemyBulletTex, std::list<EnemyBullet*>& enemyBullets_, const Player& player,bool& waitFlag,float& waitTimer);

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
	ErrorGuardian errorGuardian;
	errorGuardian.SetDebugInterface();
#pragma region DXGIファクトリーの作成
#pragma endregion
	DXGI dxgi;
	dxgi.RecruitEngineer();
	HRESULT hr;
#pragma region 使用するアダプタの決定
#pragma endregion
	OmnisTechOracle omnisTechOracle;
	omnisTechOracle.Oracle(dxgi);
#pragma region D3D12Deviceの生成
#pragma endregion
	D3D12System d3d12;
	d3d12.SelectLevel(omnisTechOracle);
#pragma region エラー警告 直ちに停止独立している
#pragma endregion
	errorGuardian.SetQueue(d3d12.GetDevice().Get());
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
	command.GetList().GetAllocator()->SetName(L"CommandAllocator");
	command.GetList().GetList()->SetName(L"commandList");
	command.GetQueue().GetQueue()->SetName(L"CommandQueue");
#pragma region SwapChain
#pragma endregion
	SwapChain swapChain;
	swapChain.Initialize(window);
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	dxgi.AssignTaskToEngineer(command.GetQueue().GetQueue().Get(), window,swapChain);

	swapChain.MakeResource();

	SRV srv;
	srv.InitializeHeap(d3d12); 
#pragma region RTVを作る
#pragma endregion
	RenderTargetView rtv;
	rtv.Initialize(&d3d12,swapChain);
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
	TachyonSync tachyonSync;
	tachyonSync.GetCGPU().Initialize(d3d12.GetDevice().Get());
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

	PSO psoLine;
	psoLine.Initialize(d3d12, PSOTYPE::Line);

	//OffScreenRendering osr;
	//osr.Initialize(d3d12,srv,1280.0f,720.0f);
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
	ImGuiManager::SetImGui(window.GetHwnd(), d3d12.GetDevice().Get(), srv.GetDescriptorHeap().GetHeap().Get());
#pragma region XAudio2初期化
#pragma endregion
	//Music music;
	//music.Initialize();
#pragma region 入力情報の初期化
#pragma endregion
	input.Initialize(window.GetWindowClass(), window.GetHwnd());

	DirectionLight light;
	light.Initialize(d3d12);
#pragma endregion
	TimeRandom::Initialize();

	//   ここからモデル系の処理

	std::unique_ptr<ModelObject> playerModel = std::make_unique<ModelObject>();
	playerModel->Initialize(d3d12, "F-14.obj");

	ModelObject bulletModel;
	bulletModel.Initialize(d3d12, "bullet.obj");

	ModelObject enemyModel;
	enemyModel.Initialize(d3d12, "enemy41-F.obj");

	ModelObject enemyBulletModel;
	enemyBulletModel.Initialize(d3d12, "enemyBullet.obj");

	ModelObject skyDomeModel;
	skyDomeModel.Initialize(d3d12, "ulthimaSky.obj");

	ModelObject groundModel;
	groundModel.Initialize(d3d12, "ground.obj");

	SpriteObject playerReticleSprite;
	playerReticleSprite.Initialize(d3d12,4.0f, 4.0f);

	Texture lineTex;
	//lineTex.Initialize(d3d12, srv, "resources/GridLine.png", 1);
	lineTex.Initialize(d3d12, srv, "resources/GridLine.png", 1);

	Texture playerTex;
	playerTex.Initialize(d3d12, srv, playerModel->GetFilePath(),2);

	Texture bulletTex;
	bulletTex.Initialize(d3d12, srv, bulletModel.GetFilePath(), 3);

	Texture enemyTex;
	enemyTex.Initialize(d3d12, srv, enemyModel.GetFilePath(), 4);

	Texture enemyBulletTex;
	enemyBulletTex.Initialize(d3d12, srv, enemyBulletModel.GetFilePath(), 5);

	Texture skyDomeTex;
	skyDomeTex.Initialize(d3d12, srv, skyDomeModel.GetFilePath(), 6);

	Texture groundTex;
	groundTex.Initialize(d3d12, srv, groundModel.GetFilePath(), 7);

	Texture playerReticleTex;
	playerReticleTex.Initialize(d3d12, srv, "resources/Reticle.png", 8);

#pragma region GridLine
	const uint32_t lineX = 50;
	const uint32_t lineXCenter = lineX / 2;
	LineObject line[lineX];
	for (uint32_t i = 0;i < lineX;++i) {
		line[i].Initialize(d3d12, 50.0f, 0.0f);
	}

	const uint32_t lineZ = 50;
	const uint32_t lineZCenter = lineZ / 2;
	LineObject lineZ_[lineZ];
	for (uint32_t i = 0;i < lineZ;++i) {
		lineZ_[i].Initialize(d3d12, 50.0f, 0.0f);
	}
#pragma endregion

	std::vector<Vector3>controlPoints_;
	controlPoints_ = {
	{0.0f, 0.0f,   0.0f},
	{0.0f, 0.0f,   5.0f},
	{0.0f, 0.0f,  10.0f},
	{0.0f, 0.0f,  15.0f},
	{0.0f, 0.0f,  20.0f},
	{0.0f, 0.0f,  25.0f},
	{0.0f, 0.0f,  30.0f},
	{0.0f, 0.0f,  35.0f},
	{0.0f, 0.0f,  40.0f},
	{0.0f, 0.0f,  45.0f}
	};
	// 線分で描画する用の頂点リスト
	std::vector<Vector3>pointsDrawing;
	// 線分の数
	const size_t segmentCount = 100;
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0;i < segmentCount + 1;++i) {
		//float t = 1.0f / segmentCount * i;
		float t = static_cast<float>(i) / segmentCount;
		Vector3 pos = CatmullRomPosition(controlPoints_,t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}
	LineObject catmullRomLine[segmentCount];
	for (size_t i = 0;i < segmentCount;++i) {
		catmullRomLine[i].Initialize(d3d12, pointsDrawing[i], pointsDrawing[i + 1]);
	}

	//   ここまでモデル系の処理
	MSG msg{};

	//   基礎的な物の処理

	CameraBase cameraBase;
	cameraBase.Initialize();
	RailCameraController railCameraController;
	railCameraController.Initialize(&cameraBase);
	railCameraController.SetRailPoints(controlPoints_);
	//Audio audio;
	//audio.SoundPlayWave(MediaAudioDecoder::DecodeAudioFile(L"resources/maou_bgm_fantasy02.mp3"));
	//music.GetBGM().LoadWAVE("resources/loop101204.wav");
	//music.GetBGM().SetPlayAudioBuf();

	//   基礎的な物の処理

	Player player;
	player.Initialize(d3d12,move(playerModel),&cameraBase);
	player.SetBullet(&bulletModel, &bulletTex);
	player.Set2DReticle(&playerReticleSprite, &playerReticleTex);
	player.SetParentMat(cameraBase.worldMat_);

	std::list<Enemy*> enemies_;
	std::list<EnemyBullet*>enemyBullets_;

	LockOn lockOn;
	lockOn.Initialize(&d3d12,playerReticleSprite, playerReticleTex);

	/*Enemy* enemy = new Enemy();
	enemy->Initialize(d3d12, enemyModel, &cameraBase, {10.0f,0.0f,40.0f});
	enemy->SetBullet(&enemyBulletModel, &enemyBulletTex);
	enemy->SetTarget(player);
	enemy->SetEnemyBullets(enemyBullets_);
	enemies_.push_back(enemy);*/
	// 敵の発生コマンド
	std::stringstream enemyPopCommand;
	bool ePopFlag = false;
	float waitTimer = 0.0f;

	// ファイルを開く
	std::ifstream file;
	file.open("enemyPop.csv");
	assert(file.is_open());
	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommand << file.rdbuf();


	std::unique_ptr<SkyDome>skyDome = std::make_unique<SkyDome>();
	skyDome->Initialize(&skyDomeModel, &cameraBase);

	std::unique_ptr<Ground>ground = std::make_unique<Ground>();
	ground->Initialize(&groundModel, &cameraBase);

	std::unique_ptr<CollisionManager> collisionManager = std::make_unique<CollisionManager>();

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
			cameraBase.UpDate();
			railCameraController.Update();
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
			UINT backBufferIndex = swapChain.GetSwapChain()->GetCurrentBackBufferIndex();

			ResourceBarrier barrier = {};
			barrier.SetBarrier(command.GetList().GetList().Get(), swapChain.GetResource(backBufferIndex).Get(),
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

			ID3D12DescriptorHeap* descriptorHeaps[] = { srv.GetDescriptorHeap().GetHeap().Get()};
			command.GetList().GetList()->SetDescriptorHeaps(1, descriptorHeaps);
			/////////////////////////////////////////////////////////////////////////
			//描画0200
			command.GetList().GetList()->RSSetViewports(1, &viewport);
			command.GetList().GetList()->RSSetScissorRects(1, &scissorRect);
			
#pragma endregion
////////////////////////////////////////////////////////////
		

			//   ここからゲームの処理↓↓↓↓

			enemies_.remove_if([](Enemy* enemy) {
				if (enemy->IsDead()) {
					delete enemy;
					return true;
				}
				return false;
			});

			enemyBullets_.remove_if([](EnemyBullet* bullet) {
				if (bullet->IsDead()) {
					delete bullet;
					return true;
				}
				return false;
			});

			

			UpdateEnemyPopCommand(enemyPopCommand, enemies_, d3d12, &cameraBase, enemyModel, enemyTex, enemyBulletModel,enemyBulletTex,enemyBullets_, player, ePopFlag, waitTimer);

			for (Enemy* enemy : enemies_) {
				enemy->Update();
			}
			for (EnemyBullet* bullet : enemyBullets_) {
				bullet->Update();
			}
			player.Update();
			lockOn.Update(&player, enemies_, cameraBase);

			skyDome->Update();
			ground->Update();

			//   当たり判定
			collisionManager->Begin();

			const std::list<PlayerBullet*>& playerBullets = player.GetBullets();
			const std::list<PlayerBulletHoming*>& playerBulletsHoming = player.GetBulletsHoming();
			collisionManager->SetColliders(&player);
			for (Enemy* enemy : enemies_) {
				collisionManager->SetColliders(enemy);
			}
			for (PlayerBullet* bullet : playerBullets) {
				collisionManager->SetColliders(bullet);
			}
			for (PlayerBulletHoming* bullet : playerBulletsHoming) {
				collisionManager->SetColliders(bullet);
			}
			for (EnemyBullet* bullet : enemyBullets_) {
				collisionManager->SetColliders(bullet);
			}
			collisionManager->CheckAllCollisions();

			//   ここまでゲームの処理↑↑↑↑
			//   ここからゲームの描画↓↓↓↓
#pragma region GridLine
			//for (uint32_t i = 0;i < lineX;++i) {
			//	Matrix4x4 world = Matrix4x4::Make::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,(i * 1.0f) - 25.0f });
			//	line[i].SetWVPData(cameraBase.DrawCamera(world), world, world);
			//	int offset = static_cast<int>(i - lineXCenter);
			//	if (offset == 0) {
			//		line[i].SetColor({ 1.0f,0.0f,0.0f,1.0f });
			//	}
			//	else if (std::abs(offset) % 10 == 0) {
			//		line[i].SetColor({ 0.25f,0.0f,0.0f,1.0f });
			//	}
			//	else {
			//		//line[i].SetColor({ 0.0f,0.0f,0.0f,1.0f });
			//		line[i].SetColor({ 0.5f,0.5f,0.5f,1.0f });
			//	}
			//	line[i].Draw(command, psoLine, light, lineTex);
			//}
			//for (uint32_t i = 0;i < lineZ;++i) {
			//	Matrix4x4 world = Matrix4x4::Make::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,Deg2Rad(90),0.0f }, { (i * 1.0f) - 25.0f,0.0f,0.0f });
			//	lineZ_[i].SetWVPData(cameraBase.DrawCamera(world), world, world);
			//	int offset = static_cast<int>(i - lineZCenter);
			//	if (offset == 0) {
			//		lineZ_[i].SetColor({ 0.0f,1.0f,0.0f,1.0f });
			//	}
			//	else if (std::abs(offset) % 10 == 0) {
			//		lineZ_[i].SetColor({ 0.0f,0.25f,0.0f,1.0f });
			//	}
			//	else {
			//		//lineZ_[i].SetColor({ 0.0f,0.0f,0.0f,1.0f });
			//		lineZ_[i].SetColor({ 0.5f,0.5f,0.5f,1.0f });
			//	}
			//	lineZ_[i].Draw(command, psoLine, light, lineTex);
			//}
#pragma endregion
			//   CatmullRom曲線の描画
			for (size_t i = 0;i < segmentCount;++i) {
				Matrix4x4 world = Matrix4x4::Make::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
				catmullRomLine[i].SetWVPData(cameraBase.DrawCamera(world), world, world);
				catmullRomLine[i].SetColor({ 1.0f,0.0f,0.0f,1.0f });
				catmullRomLine[i].Draw(command, psoLine, light, lineTex);
			}
			skyDome->Draw(command, pso, light, skyDomeTex);	
			//ground->Draw(command, pso, light, groundTex);
			player.Draw(command,pso,light,playerTex);
			for (Enemy* enemy : enemies_) {
				enemy->Draw(command, pso, light, enemyTex);
			}
			for (EnemyBullet* bullet : enemyBullets_) {
				bullet->Draw(cameraBase, command, pso, light, enemyBulletTex);
			}

			lockOn.Draw(command, pso, light);

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
			swapChain.GetSwapChain()->Present(1, 0);

			tachyonSync.GetCGPU().Update(command.GetQueue().GetQueue().Get());


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
	tachyonSync.GetCGPU().UnLoad();
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	//music.UnLoad();

#ifdef _DEBUG
	//debugController->Release();
#endif // _DEBUG
	CloseWindow(window.GetHwnd());

	//COMの初期化を解除
	CoUninitialize();
	return 0;
}

void PopEnemy(std::list<Enemy*>& enemies_, D3D12System& d3d12, CameraBase* camera, ModelObject& enemyModel, Texture& enemyTex, ModelObject& enemyBulletModel, Texture& enemyBulletTex, std::list<EnemyBullet*>& enemyBullets_, const Vector3& pos, const Player& player,uint32_t movePattern) {
	// 敵を生成
	Enemy* enemy = new Enemy();
	enemy->Initialize(d3d12, enemyModel, camera, pos);
	enemy->SetMovePattern(movePattern);
	enemy->SetBullet(&enemyBulletModel, &enemyBulletTex);
	enemy->SetTarget(player);
	enemy->SetEnemyBullets(enemyBullets_);
	enemies_.push_back(enemy);
}

void LoadEnemyPopData(std::stringstream& command, std::string filepath) {
	// ファイルを開く
	std::ifstream file;
	file.open(filepath);
	assert(file.is_open());
	// ファイルの内容を文字列にストリームにコピー
	command << file.rdbuf();
	// ファイルを閉じる
	file.close();
}

void UpdateEnemyPopCommand(std::stringstream& command, std::list<Enemy*>& enemies_, D3D12System& d3d12, CameraBase* camera, ModelObject& enemyModel, Texture& enemyTex, ModelObject& enemyBulletModel, Texture& enemyBulletTex, std::list<EnemyBullet*>& enemyBullets_, const Player& player, bool& waitFlag, float& waitTimer) {
	// 待機処理
	if (waitFlag) {
		waitTimer--;
		if (waitTimer <= 0) {
			waitFlag = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(command, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_Stream(line);

		std::string word;
		// ,区切りで行の先頭文字を取得
		getline(line_Stream, word, ',');
		if (word.find("//") == 0) {
			// コメント行は飛ばす
			continue;
		}
		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_Stream, word, ',');
			float x = static_cast<float>(std::atof(word.c_str()));
			// y座標
			getline(line_Stream, word, ',');
			float y = static_cast<float>(std::atof(word.c_str()));
			// z座標
			getline(line_Stream, word, ',');
			float z = static_cast<float>(std::atof(word.c_str()));
			// movePattern
			getline(line_Stream, word, ',');
			uint32_t movePattern = static_cast<uint32_t>(std::atoi(word.c_str()));
			// 生成
			Vector3 pos = { x,y,z };
			PopEnemy(enemies_, d3d12, camera, enemyModel, enemyTex, enemyBulletModel,enemyBulletTex,enemyBullets_, pos, player,movePattern);
		}
		// WAIT
		else if (word.find("WAIT") == 0) {
			getline(line_Stream, word, ',');

			// 待ち時間
			int32_t waitTime = std::atoi(word.c_str());

			// 待機時間
			waitFlag = true;
			waitTimer = static_cast<float>(waitTime);

			break;
		}
	}
}

//void checkcollisionpair(collider* collidera, collider* colliderb) {
//	if ((collidera->getmytype() & colliderb->getyourtype()) == 0 ||
//		(colliderb->getmytype() & collidera->getyourtype()) == 0) {
//		return;
//	}
//
//	vector3 posa = collidera->getworldposition();
//	vector3 posb = colliderb->getworldposition();
//	float length = length(posa - posb);
//	if (collidera->getradius() + colliderb->getradius() >= length) {
//		collidera->oncollision();
//		colliderb->oncollision();
//	}
//}
