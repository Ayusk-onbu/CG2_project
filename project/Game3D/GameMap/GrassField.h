#pragma once
#include "../../Engine/Objects/Primitive/Grass/Grass.h"
#include "EmitterSystem.h"

class GrassField {
public:
	void Initialize(Fngine* engine, uint32_t maxGrass, const std::string& densityMapName);

	void Update();
private:
	std::vector<GrassObjectData> staticGrassList_;
	EmitterConfig grassConfig_;
};