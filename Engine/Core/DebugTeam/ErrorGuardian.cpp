#include "ErrorGuardian.h"
#pragma comment(lib,"Dbghelp.lib")//ClashHandler06

//CrashHandler06
LONG WINAPI ErrorGuardian::ExportDump(EXCEPTION_POINTERS* exception) {
	//ファイルはクラッシュしたexeまたは、ドイツの同一のexeとソースファイルと同じ場所
	//Dumpを出力する↓↓↓

	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processId(このexeとId)とクラッシュ(例外)の発生したthreatIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation = { 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//Dumpの出力↑↑↑

	//他に関連づけられているSHE例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

void ErrorGuardian::SetDebugInterface() {
#ifdef _DEBUG
	debugController_ = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		//でばっふレイヤーを有効かする
		debugController_->EnableDebugLayer();
		//さらにGPU側でもチェックポイントを行うようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif 
}

void ErrorGuardian::SetQueue(Microsoft::WRL::ComPtr <ID3D12Device>& device) {
#ifdef _DEBUG
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
		//やばいエラー時に止まる
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる(全部の情報を出すために)
		//infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11出のDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/09885245/directx-12-application-is-crashing-in-window-ww
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//確定したメッセージの表示を抑制する
		infoQueue_->PushStorageFilter(&filter);
		//解放
		//infoQueue_->Release();
	}
#endif 
}