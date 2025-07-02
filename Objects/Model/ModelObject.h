#pragma once
#include "ObjectBase.h"

class ModelObject
{
public:
	void Initialize(D3D12System d3d12, const std::string& filename, const std::string& directoryPath = "resources");

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);

	void SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv);

	void SetColor(Vector4& color);

	std::string GetFilePath() { return modelData_.material.textureFilePath; }
private:
	void InitializeResource(D3D12System d3d12,const std::string& filename, const std::string& directoryPath);

	void InitializeData();

	ModelData LoadObjFile(const std::string& filename, const std::string& directoryPath);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
private:
	ObjectBase object_;
	ModelData modelData_;
};

