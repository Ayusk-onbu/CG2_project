#include "RasterizerState.h"

void RasterizerState::SetDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill) {
	rasterizerDesc_.CullMode = cull;
	rasterizerDesc_.FillMode = fill;
}