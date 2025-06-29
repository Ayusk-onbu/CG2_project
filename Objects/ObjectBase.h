#pragma once
#include <wrl.h>
#include "ResourceFunc.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "VertexData.h"
#include "ModelData.h"
#include "Transform.h"

#include "D3D12System.h"
#include "TheOrderCommand.h"
#include "PipelineStateObject.h"
#include "DirectionLight.h"
#include "Texture.h"

class ObjectBase
{
public:

	void DrawBase(TheOrderCommand& command,PSO& pso,DirectionLight& light,Texture& tex);
	void DrawBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);

	void DrawIndexBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex);
	void DrawIndexBase(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex);


	void InitializeMD(Vector4 color, bool isLight);

	void InitializeWVPD();
public:
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource_;

public:
	VertexData* vertexData_;
	uint32_t* indexData_;
	Material* materialData_;
	TransformationMatrix* wvpData_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
};

