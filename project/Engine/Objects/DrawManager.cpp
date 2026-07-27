#include "DrawManager.h"

void DrawManager::Initialize(Fngine* engine) {
	sphere_ = std::make_unique<PrimitiveSphere>();
	sphere_->Initialize(engine,5000);

	cylinder_ = std::make_unique<PrimitiveCylinder>();
	cylinder_->Initialize(engine,5000);

	line_ = std::make_unique<PrimitiveLine>();
	line_->Initialize(engine, 10000);

	magicCircle_ = std::make_unique<MagicCircle>();
	magicCircle_->Initialize(engine, 100);

	grass_ = std::make_unique<Grass>();
	grass_->Initialize(engine, 5000);

	column_ = std::make_unique<Column>();
	column_->Initialize(engine, 100);
}

void DrawManager::Draw() {
	grass_->DrawInstanced();

	column_->DrawInstanced();

	line_->DrawInstanced();

	sphere_->DrawInstanced();

	magicCircle_->DrawInstanced();

	cylinder_->DrawInstanced();
}