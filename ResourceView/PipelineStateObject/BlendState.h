#pragma once
#include <d3d12.h>

enum class USECOLOR {
	Red   = 0b1000,
	Green = 0b0100,
	Blue  = 0b0010,
	Alpha = 0b0001,
	All   = 0b1111,
};

class BlendState
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="color 使う色を決める"></param>
	void Initialize(USECOLOR color);

	D3D12_BLEND_DESC GetDesc() { return blendDesc_; }
private:
	D3D12_BLEND_DESC blendDesc_ = {};
};

