#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dbghelp.h>//CrashHandler06
#include <strsafe.h>//Dumpを出力06

class ErrorGuardian
{
public:

	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

	static void SetDebugInterface();

	static void SetQueue(Microsoft::WRL::ComPtr <ID3D12Device> device);

private:
	static Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_;
	static Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue_;
};

