#include "BlendState.h"

void BlendState::Initialize(USECOLOR color) {
	if (color == USECOLOR::All) {
		blendDesc_.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;
	}
}