#pragma once
#include <wrl.h>
#include <dxgi1_6.h>

class OmnisTechOracle
{
public:
	static void Oracle();
public:
	static Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_;
};

