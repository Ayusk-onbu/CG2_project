#include "DrawManager.h"

void DrawManager::Initialize(Fngine* engine) {
	sphere_ = std::make_unique<PrimitiveSphere>();
	sphere_->Initialize(engine,5000);

	cylinder_ = std::make_unique<PrimitiveCylinder>();
	cylinder_->Initialize(engine,5000);
}

void DrawManager::Draw() {
	sphere_->DrawInstanced();
	cylinder_->DrawInstanced();
}