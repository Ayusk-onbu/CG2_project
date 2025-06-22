#pragma once
#include "ObjectBase.h"

class ModelObject
{
public:
	ModelObject() {};

	void Initialize(D3D12System d3d12, const std::string& filename, const std::string& directoryPath = "resources");

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex, uint32_t num = 1);

	void SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv);

	std::string GetFilePath() { return modelData_.material.textureFilePath; }

	void SetColor(Vector4& color) { object_.materialData_->color = color; }
private:
	void InitializeResource(D3D12System d3d12,const std::string& filename, const std::string& directoryPath);

	void InitializeData();

	ModelData LoadObjFile(const std::string& filename, const std::string& directoryPath);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
private:
	ObjectBase object_;
	ModelData modelData_;
};

