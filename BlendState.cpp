#include "BlendState.h"

void BlendState::Initialize(USECOLOR color) {
	if (color == USECOLOR::All) {
		blendDesc_.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
}