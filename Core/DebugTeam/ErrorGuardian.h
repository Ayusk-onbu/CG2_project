#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dbghelp.h>//CrashHandler06
#include <strsafe.h>//Dumpを出力06

class ErrorGuardian
{
public:

	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

	void SetDebugInterface();

	void SetQueue(Microsoft::WRL::ComPtr <ID3D12Device>& device);

private:
	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController_;
	Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue_;
};

