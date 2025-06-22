//#include <windows.h>
//#include <cstdint>
//#include <d3d12.h>
#include <dxgi1_6.h>
//#include <cassert>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>//ファイルに書いたり読んだりするライブラリ
#include <sstream>
#include <chrono>//時間を扱うライブラリ
//#include <dbghelp.h>//CrashHandler06
//#include <strsafe.h>//Dumpを出力06
#include <dxgidebug.h>//0103 ReportLiveobject
//#include <dxcapi.h>//DXCの初期化
#include <cmath>
#include <wrl.h>

#include "Window.h"
#include "ErrorGuardian.h"
#include "Log.h"
#include "DXGI.h"
#include "D3D12System.h"

#include "CommandQueue.h"
#include "CommandList.h"

#include "TheOrderCommand.h"

#include "SwapChain.h"
#include "TachyonSync.h"
#include "OmnisTechOracle.h"
#include "RenderTargetView.h"

#include "PipelineStateObject.h"
#include "Texture.h"


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
#include "DebugCamera.h"



#include "Player.h"
#include "Enemy.h"
#include "MapChip.h"
#include "SkyDome.h"
#include "DeathParticle.h"
#include "Text.h"

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"dxgi.lib")
//#pragma comment(lib,"Dbghelp.lib")//ClashHandler06
#pragma comment(lib,"dxguid.lib")//0103 ReportLiveobject
//#pragma comment(lib,"dxcompiler.lib")//DXCの初期化


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

#pragma region 初期化
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
#pragma region XAudio2初期化
#pragma endregion
	/*Music music;
	music.Initialize();*/
#pragma region 入力情報の初期化
#pragma endregion
	input.Initialize(window.GetWindowClass(), window.GetHwnd());

	DirectionLight light;
	light.Initialize(d3d12);
#pragma endregion

	ModelObject playerModel;
	playerModel.Initialize(d3d12, "player.obj");
	
	ModelObject blockModel;
	blockModel.Initialize(d3d12, "brickBlock.obj");
	
	/*const int enemyNum = 3;
	std::vector<ModelObject> enemyModel(3);
	for (int i = 0;i < enemyNum;++i) {
		enemyModel[i].Initialize(d3d12, "player.obj");
	}*/
	//enemyModel[0].Initialize(d3d12, "player.obj");
	//enemyModel[1].Initialize(d3d12, "player.obj");
	//enemyModel[2].Initialize(d3d12, "player.obj");
	ModelObject enemyModel;
	enemyModel.Initialize(d3d12, "player.obj");
	
	ModelObject skyDomeModel;
	skyDomeModel.Initialize(d3d12, "skydome.obj");

	ModelObject deathParticleModel;
	deathParticleModel.Initialize(d3d12, "deathParticle.obj");

	ModelObject textModel;
	textModel.Initialize(d3d12, "Text.obj");

	SpriteObject feedInOut;
	feedInOut.Initialize(d3d12,1280.0f,720.0f);

	Texture tex;
	tex.Initialize(d3d12, "resources/uvChecker.png", 1);
	Texture playerTex;
	playerTex.Initialize(d3d12, playerModel.GetFilePath(), 2);
	Texture blockTex;
	blockTex.Initialize(d3d12, blockModel.GetFilePath(), 3);
	Texture skyDomeTex;
	skyDomeTex.Initialize(d3d12, skyDomeModel.GetFilePath(), 4);
	Texture deathParticleTex;
	deathParticleTex.Initialize(d3d12, deathParticleModel.GetFilePath(), 5);
	Texture textTex;
	textTex.Initialize(d3d12, textModel.GetFilePath(), 6);

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
	////////////////////////////////////////////////////////////

	ImGuiManager::SetImGui(window.GetHwnd(),d3d12.GetDevice().Get(),SRV::descriptorHeap_.GetHeap().Get());

	MSG msg{};

	DebugCamera debugCamera;
	debugCamera.Initialize();

	Camera camera;
	camera.Initialize();

	bool isDebug = false;

	MapChipField mapChip;
	mapChip.LoadMapChipCsv("resources/mapChip.csv");

	Player player;
	Vector3 playerPosition = mapChip.GetBlockPositionByIndex(2, 18);
	player.Initialize(&playerModel,playerPosition);
	player.SetMapChipField(&mapChip);


	/*Enemy enemy[enemyNum];
	for (int i = 0;i < enemyNum;++i) {
		Vector3 enemyPosition = mapChip.GetBlockPositionByIndex(10 + i, 18);
		enemy[i].Initialize(&enemyModel[i], enemyPosition);
	}*/
	//Vector3 enemyPosition = mapChip.GetBlockPositionByIndex(10 + 1, 18);
	//enemy[0].Initialize(&enemyModel[0], enemyPosition);
	//enemyPosition = mapChip.GetBlockPositionByIndex(10 + 2, 18);
	//enemy[1].Initialize(&enemyModel[1], enemyPosition);
	//enemyPosition = mapChip.GetBlockPositionByIndex(10 + 3, 18);
	//enemy[2].Initialize(&enemyModel[2], enemyPosition);
	Enemy enemy;
	Vector3 enemyPosition = mapChip.GetBlockPositionByIndex(10 , 18);
	enemy.Initialize(&enemyModel, enemyPosition);

	SkyDome skyDome;
	skyDome.Initialize(&skyDomeModel);

	DeathParticle deathParticle;
	deathParticle.Initialize(&deathParticleModel, playerPosition);

	Text text;
	text.Initialize(&textModel);

	int time = 120;
	Vector4 color = { 0.0f,0.0f,0.0f,1.0f };

	//Block blocks[20][100];
	std::vector<std::vector<Transform*>> worldTransformBlocks_;
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
				Transform* worldTransform = new Transform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translate = mapChip.GetBlockPositionByIndex(j, i);
			}
		}
	}
	CameraController cameraController;
	cameraController.Initialize(&camera);
	cameraController.SetTarget(&player);
	cameraController.Reset();
	// 移動範囲の指定
	Rect area = { 0, 100, 0, 100 };
	area.left = 11;
	area.right = mapChip.GetBlockPositionByIndex(100, 0).x;
	area.bottom = mapChip.GetBlockPositionByIndex(0, 13).y;
	area.top = mapChip.GetBlockPositionByIndex(0, 20).y - 10;
	cameraController.SetMovableArea(area);

	enum Scene {
		TITLE,
		GAME
	};
	int scene = 0;

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

			// キー情報の更新があった
			input.Update();
			

////////////////////////////////////////////////////////////
#pragma region コマンドを積み込んで確定させる
	//これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = SwapChain::swapChain_->GetCurrentBackBufferIndex();

			ResourceBarrier barrier = {};
			barrier.SetBarrier(command.GetList().GetList().Get(), SwapChain::GetResource(backBufferIndex).Get(),
				D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			//描画先のRTVを設定する
			command.GetList().GetList()->OMSetRenderTargets(1, &rtv.GetHandle(backBufferIndex), false, nullptr);
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
			cameraController.Update();
			debugCamera.UpData();
			camera.UpData();
			time -= 1;
			color.w = time / 120.0f < 0 ? 0 : time / 120.0f;
			feedInOut.SetColor(color);

			switch (scene) {
			case TITLE:

				
				if (InputManager::GetKey().HoldKey(DIK_SPACE)) {
					time += 2;
				}
				if (time >= 121) {
					scene = 1;
					color = { 1.0f,1.0f,1.0f,1.0f };
					feedInOut.SetColor(color);
				}

				text.UpDate();
				ImGui::Begin("text");
				ImGuiManager::CreateImGui("Tra",text.GetTranslate().translate,1.0f,10.0f);
				ImGuiManager::CreateImGui("Getcolor", feedInOut.GetColor(), 0.0f, 1.0f);
				ImGuiManager::CreateImGui("color", color, 0.0f, 1.0f);
				ImGui::End();

				if (isDebug) {
					text.Draw(debugCamera, command, pso, light, textTex);
					player.Draw(debugCamera, command, pso, light, playerTex);
					if (InputManager::GetKey().ReleaseKey(DIK_RETURN)) {
						isDebug = false;
					}
				}
				else {
					text.Draw(camera, command, pso, light, textTex);
					player.Draw(camera, command, pso, light, playerTex);
					if (InputManager::GetKey().ReleaseKey(DIK_RETURN)) {
						isDebug = true;
					}
				}
				

				break;
			case GAME:
				

				// 更新処理



				if (!player.GetIsDeathFlag())
					player.UpDate();
				enemy.UpDate();
				/*for (int i = 0;i < enemyNum;++i) {
					enemy[i].UpDate();
				}*/
				/*enemy[0].UpDate();
				enemy[1].UpDate();
				enemy[2].UpDate();*/
				skyDome.UpDate();

				AABB aabb1, aabb2;
				aabb1 = AABB::World2AABB(player.GetWorldTransform().translate);
				aabb2 = AABB::World2AABB(enemy.GetWorldTransform().translate);

				if (AABB::IsHitAABB2AABB(aabb1, aabb2)) {
					if (!player.GetIsDeathFlag())
						player.OnCollision(&enemy);
					enemy.OnCollision(&player);

					deathParticle.UpDate();
				}



				// 描画処理
				if (isDebug) {
					skyDome.Draw(debugCamera, command, pso, light, skyDomeTex);
					if (AABB::IsHitAABB2AABB(aabb1, aabb2)) {
						if (!player.GetIsDeathFlag())
							player.OnCollision(&enemy);
						enemy.OnCollision(&player);

						deathParticle.Draw(debugCamera, command, pso, light, deathParticleTex);
					}

					for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
						for (Transform* worldTransformBlock : worldTransformBlockLine) {
							if (!worldTransformBlock)
								continue;
							ImGui::Begin("Block");
							ImGuiManager::CreateImGui("Transform", worldTransformBlock->translate, -10.0f, 100.0f);
							ImGui::End();
							Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransformBlock->scale, worldTransformBlock->rotate, worldTransformBlock->translate);
							Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale({ 1.0f,1.0f,1.0f });
							uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(0.0f));
							uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate({ 0.0f,0.0f,0.0f }));
							blockModel.SetWVPData(debugCamera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
							blockModel.Draw(command, pso, light, blockTex);

						}
					}
					if (!player.GetIsDeathFlag())
						player.Draw(debugCamera, command, pso, light, playerTex);
					/*for (int i = 0;i < enemyNum;++i) {
						enemy[i].Draw(debugCamera, command, pso, light, playerTex);
					}*/
					/*enemy[0].Draw(debugCamera, command, pso, light, playerTex);
					enemy[1].Draw(debugCamera, command, pso, light, playerTex);
					enemy[2].Draw(debugCamera, command, pso, light, playerTex);*/
					enemy.Draw(debugCamera, command, pso, light, playerTex);
					if (InputManager::GetKey().ReleaseKey(DIK_RETURN)) {
						isDebug = false;
					}
				}
				else {
					skyDome.Draw(camera, command, pso, light, skyDomeTex);
					if (AABB::IsHitAABB2AABB(aabb1, aabb2)) {
						if (!player.GetIsDeathFlag())
							player.OnCollision(&enemy);
						enemy.OnCollision(&player);
						deathParticle.Draw(camera, command, pso, light, deathParticleTex);
					}

					for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
						for (Transform* worldTransformBlock : worldTransformBlockLine) {
							if (!worldTransformBlock)
								continue;
							ImGui::Begin("Block");
							ImGuiManager::CreateImGui("Transform", worldTransformBlock->translate, -10.0f, 100.0f);
							ImGui::End();
							Matrix4x4 worldMatrix = Matrix4x4::Make::Affine(worldTransformBlock->scale, worldTransformBlock->rotate, worldTransformBlock->translate);
							Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale({ 1.0f,1.0f,1.0f });
							uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(0.0f));
							uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate({ 0.0f,0.0f,0.0f }));
							blockModel.SetWVPData(camera.DrawCamera(worldMatrix), worldMatrix, uvTransformMatrix);
							blockModel.Draw(command, pso, light, blockTex);
						}
					}
					if (!player.GetIsDeathFlag())
						player.Draw(camera, command, pso, light, playerTex);
					/*for (int i = 0;i < enemyNum;++i) {
						enemy[i].Draw(debugCamera, command, pso, light, playerTex);
					}*/
					/*enemy[0].Draw(debugCamera, command, pso, light, playerTex);
					enemy[1].Draw(debugCamera, command, pso, light, playerTex);
					enemy[2].Draw(debugCamera, command, pso, light, playerTex);*/
					enemy.Draw(camera, command, pso, light, playerTex);
					if (InputManager::GetKey().ReleaseKey(DIK_RETURN)) {
						isDebug = true;
					}
				}
				break;
			}
			Matrix4x4 worldMatrixSprite = Matrix4x4::Make::Affine({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
			Matrix4x4 viewMatrixSprite = Matrix4x4::Make::Identity();
			Matrix4x4 projectionMatrixSprite = Matrix4x4::Make::OrthoGraphic(0.0f, 0.0f, static_cast<float>(window.GetWindowRect().right), static_cast<float>(window.GetWindowRect().bottom), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = Matrix4x4::Multiply(worldMatrixSprite, Matrix4x4::Multiply(viewMatrixSprite, projectionMatrixSprite));

			Matrix4x4 uvTransformMatrix = Matrix4x4::Make::Scale({ 1.0f,1.0f,1.0f });
			uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::RotateZ(0.0f));
			uvTransformMatrix = Matrix4x4::Multiply(uvTransformMatrix, Matrix4x4::Make::Translate({ 0.0f,0.0f,0.0f }));

			feedInOut.SetWVPData(worldViewProjectionMatrixSprite, worldMatrixSprite, uvTransformMatrix);
			feedInOut.Draw(command, pso, light, deathParticleTex);
			
			//描画
			ImGuiManager::EndFrame(command.GetList().GetList());

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
	for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Transform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
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
