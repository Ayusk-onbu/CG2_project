#pragma once
#include <dxcapi.h>//DXCの初期化
#include <wrl.h>

class DXC
{
public:
	void Initialize();

	Microsoft::WRL::ComPtr<IDxcUtils>& GetUtils() { return dxcUtils_; }
	Microsoft::WRL::ComPtr<IDxcCompiler3>& GetCompiler() { return dxcCompiler_; }
	Microsoft::WRL::ComPtr<IDxcIncludeHandler>& GetIncludeHandle() { return includeHandler_; }
private:
	Microsoft::WRL::ComPtr<IDxcUtils>dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3>dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler>includeHandler_;
};

