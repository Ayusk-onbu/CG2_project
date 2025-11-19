#pragma once
#include <d3d12.h>

struct RasterizerSettings {
	D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK;
	D3D12_FILL_MODE FillMode = D3D12_FILL_MODE_SOLID;
	BOOL            FrontCounterClockwise = FALSE; // Direct3Dのデフォルト
	INT             DepthBias = 0;
	FLOAT           SlopeScaledDepthBias = 0.0f;
};

class RasterizerState
{
public:
	void SetDesc(RasterizerSettings settings);

	D3D12_RASTERIZER_DESC& GetDesc() { return rasterizerDesc_; }
private:
	D3D12_RASTERIZER_DESC rasterizerDesc_ = {};
};

