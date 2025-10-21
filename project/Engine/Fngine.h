#pragma once
#include <dxgi1_6.h>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>//ファイルに書いたり読んだりするライブラリ
#include <sstream>
#include <chrono>//時間を扱うライブラリ
#include <dxgidebug.h>//0103 ReportLiveobject
#include <cmath>
#include <wrl.h>
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
#include "DirectionLight.h"
#include "ImGuiManager.h"
#include "Structures.h"
#include "ResourceBarrier.h"
#include "InputManager.h"
#include "Music.h"
#include "externals/DirectXTex/DirectXTex.h"

#define pi float(3.14159265358979323846f)

class Fngine
{
public:
	Fngine();
	~Fngine();
public:
	void Initialize();
	void BeginOSRFrame();
	void EndOSRFrame();
	void BeginFrame();
	void EndFrame();
	D3D12System& GetD3D12System() { return d3d12_; }
	TheOrderCommand& GetCommand() { return command_; }
	SRV& GetSRV() { return srv_; }
	PSO& GetPSO() { return pso_; }
	DirectionLight& GetLight() { return light_; }
private:

	int32_t kClienWidth_ = 1280;
	int32_t kClienHeight_ = 720;

private:

	Window window_;
	D3D12System d3d12_;
	DXGI dxgi_;
	ErrorGuardian errorGuardian_;
	TheOrderCommand command_;
	OmnisTechOracle omnisTechOracle_;
	TachyonSync tachyonSync_;

	SRV srv_;
	RTV rtv_;
	DSV dsv_;
	PSO pso_;
	OffScreenRendering osr_;
	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;
	SwapChain swapChain_;

	Music music_;

	DirectionLight light_;
};

