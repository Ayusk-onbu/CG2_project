#pragma once
#include <string>
#include <vector>
#include "VertexData.h"

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};