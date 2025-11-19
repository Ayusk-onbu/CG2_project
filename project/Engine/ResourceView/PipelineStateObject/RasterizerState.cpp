#include "RasterizerState.h"

void RasterizerState::SetDesc(RasterizerSettings settings) {
	rasterizerDesc_.CullMode = settings.CullMode;
	rasterizerDesc_.FillMode = settings.FillMode;
	rasterizerDesc_.FrontCounterClockwise = settings.FrontCounterClockwise;
	rasterizerDesc_.DepthBias = settings.DepthBias;
	rasterizerDesc_.SlopeScaledDepthBias = settings.SlopeScaledDepthBias;
}