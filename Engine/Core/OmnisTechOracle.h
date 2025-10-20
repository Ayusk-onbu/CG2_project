#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include "DXGI.h"

class OmnisTechOracle
{
public:
	void Oracle(DXGI& dxgi);
	Microsoft::WRL::ComPtr <IDXGIAdapter4>& GetAdapter() {return useAdapter_;};
private:
	Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_;
};

