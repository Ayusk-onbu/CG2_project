#include "BlendState.h"

void BlendState::SetBlendMode(BLENDMODE blendMode) {
	blendMode_ = blendMode;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	if (BLENDMODE::AlphaBlend == blendMode_) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	}
	else if (BLENDMODE::None == blendMode_) {
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc_ = blendDesc;
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	}
	else if (BLENDMODE::Additive == blendMode_) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	}
	else if (BLENDMODE::Multiplicative == blendMode_) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	}
	else if (BLENDMODE::Subtractive == blendMode_) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	}
	else if (BLENDMODE::ScreenBlend == blendMode_) {
		blendDesc_.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	}
}

void BlendState::Initialize(USECOLOR color) {
	if (color == USECOLOR::All) {
		blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		SetBlendMode(BLENDMODE::ScreenBlend);
	}
}



// 完全に無視（0）
//D3D12_BLEND_ZERO

// そのまま使う（1）
//D3D12_BLEND_ONE

// ソースのアルファ値（ピクセルシェーダーの出力α）
//D3D12_BLEND_SRC_ALPHA

// 1 - ソースのアルファ値
//D3D12_BLEND_INV_SRC_ALPHA

// デスティネーション（既存ピクセル）の色
//D3D12_BLEND_DEST_COLOR

// 1 - デスティネーションの色
//D3D12_BLEND_INV_DEST_COLOR

// ソースの色
//D3D12_BLEND_SRC_COLOR

// 1 - ソースの色
//D3D12_BLEND_INV_SRC_COLOR

// デスティネーションのアルファ値
//D3D12_BLEND_DEST_ALPHA

// 1 - デスティネーションのアルファ値
//D3D12_BLEND_INV_DEST_ALPHA