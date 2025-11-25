#include "InputLayout.h"

void InputLayout::Initialize(
	const D3D12_INPUT_ELEMENT_DESC* pElementDescs,
	UINT numElements) 
{
	// 外部で設定したDescを適用
	inputLayoutDesc_.pInputElementDescs = pElementDescs;
	// 要素数を適用
	inputLayoutDesc_.NumElements = numElements;
}