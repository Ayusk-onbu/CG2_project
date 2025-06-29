#pragma once
#include <d3d12.h>

class InputLayout
{
public:
	/// <summary>
	/// VerTexShaderに送りたい情報の設定
	/// </summary>
	void Initialize();

	D3D12_INPUT_LAYOUT_DESC GetDesc() { return inputLayoutDesc_; }
private:
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_ = {};
};

