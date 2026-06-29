#include "DrawManager.h"

void DrawManager::Initialize(Fngine* engine) {
	sphere_ = std::make_unique<PrimitiveSphere>();
	sphere_->Initialize(engine,100);
}

void DrawManager::Draw() {
	sphere_->DrawInstanced();
}