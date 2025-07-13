#include "Fngine.h"

void Fngine::Initialize() {
	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//誰も補足しなかった場合に（Unhandled）、補足する関数を登録
	//main関数はじまってすぐに登録するといい
	SetUnhandledExceptionFilter(ErrorGuardian::ExportDump);
	Log::Initialize();

	window_.Initialize(L"CG2ClassName", L"CG2");
	errorGuardian_.SetDebugInterface();
	dxgi_.RecruitEngineer();
	omnisTechOracle_.Oracle(dxgi_);
	d3d12_.SelectLevel(omnisTechOracle_);
	errorGuardian_.SetQueue(d3d12_.GetDevice().Get());
	command_.GetQueue().SetDescD();
	HRESULT hr;
	hr = d3d12_.GetDevice()->CreateCommandQueue(&command_.GetQueue().GetDesc(), IID_PPV_ARGS(&command_.GetQueue().GetQueue()));
	if (FAILED(hr)) {
		Log::ViewFile("CreateCommandQueue failed!!!\n");
	}
	hr = d3d12_.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&command_.GetList().GetAllocator()));
	if (FAILED(hr)) {
		Log::ViewFile("CreateCommandAllocator failed!!!\n");
	}
	hr = d3d12_.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_.GetList().GetAllocator().Get(), nullptr,
		IID_PPV_ARGS(&command_.GetList().GetList()));
	if (FAILED(hr)) {
		Log::ViewFile("CreateCommandList failed!!!\n");
	}
	swapChain_.Initialize(window_);
	dxgi_.AssignTaskToEngineer(command_.GetQueue().GetQueue().Get(), window_,swapChain_);

	srv_.InitializeHeap(d3d12_);
	rtv_.Initialize(&d3d12_,swapChain_);
	dsv_.InitializeHeap(d3d12_);
	dsv_.MakeResource(d3d12_, kClienWidth_, kClienHeight_);
	d3d12_.GetDevice()->CreateDepthStencilView(dsv_.GetResource().Get(), &dsv_.GetDSVDesc(), dsv_.GetHeap().GetHeap()->GetCPUDescriptorHandleForHeapStart());
	tachyonSync_.GetCGPU().Initialize(d3d12_.GetDevice().Get());
	pso_.Initialize(d3d12_, PSOTYPE::Normal);
	osr_.Initialize(d3d12_,srv_, float(kClienWidth_), float(kClienHeight_));
	viewport_.Width = static_cast<float>(window_.GetWindowRect().right);
	viewport_.Height = static_cast<float>(window_.GetWindowRect().bottom);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	scissorRect_.left = 0;
	scissorRect_.right = window_.GetWindowRect().right;
	scissorRect_.top = 0;
	scissorRect_.bottom = window_.GetWindowRect().bottom;

	music_.Initialize();
	input_.Initialize(window_.GetWindowClass(), window_.GetHwnd());

	ImGuiManager::SetImGui(window_.GetHwnd(), d3d12_.GetDevice().Get(), srv_.GetDescriptorHeap().GetHeap().Get());

	light_.Initialize(d3d12_);
}

void Fngine::BeginOSRFrame() {

}

void Fngine::EndOSRFrame() {

}

void Fngine::BeginFrame() {

}

void Fngine::EndFrame() {

}