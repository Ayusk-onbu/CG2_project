#pragma once
#include "ObjectBase.h"

class ModelObject
{
public:
	void Initialize(D3D12System d3d12, const std::string& filename, const std::string& directoryPath = "resources");
	void Initialize(D3D12System d3d12, ModelData& modelData);

	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);

	void SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv);
	void SetColor(Vector4& color);

	void SetObject(ObjectBase& object) { object_ = object; }
	void SetModelData(ModelData& modelData) { modelData_ = modelData; }

	std::string& GetFilePath() { return modelData_.material.textureFilePath; }
	ModelData& GetModelData() { return modelData_; }
private:
	void InitializeResource(D3D12System d3d12,const std::string& filename, const std::string& directoryPath);
	void InitializeResource(D3D12System d3d12, ModelData& modelData);

	void InitializeData();

	ModelData LoadObjFile(const std::string& filename, const std::string& directoryPath);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
private:
	ObjectBase object_;
	ModelData modelData_;
};

