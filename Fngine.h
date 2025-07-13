#pragma once
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
//#include "CommandQueue.h"
//#include "CommandList.h"
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
#include "DebugCamera.h"


#include "externals/DirectXTex/DirectXTex.h"

class Fngine
{
public:
	void Initialize();
	void BeginOSRFrame();
	void EndOSRFrame();
	void BeginFrame();
	void EndFrame();
	D3D12System& GetD3D12System() { return d3d12_; }
private:

	int32_t kClienWidth_;
	int32_t kClienHeight_;
	

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
	InputManager input_;

	DirectionLight light_;
};

