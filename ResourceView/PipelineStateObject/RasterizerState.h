#pragma once
#include <d3d12.h>

class RasterizerState
{
public:
	void SetDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill);

	D3D12_RASTERIZER_DESC& GetDesc() { return rasterizerDesc_; }
private:
	D3D12_RASTERIZER_DESC rasterizerDesc_ = {};
};

