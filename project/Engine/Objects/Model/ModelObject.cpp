#include "ModelObject.h"
#include <sstream>
#include "Log.h"

//==========-+-==========
// Initialize Function
//==========-+-==========

ModelObject::~ModelObject() {
	/*if (vertexResource_)   vertexResource_.Reset();
	if (materialResource_) materialResource_.Reset();
	if (wvpResource_)      wvpResource_.Reset();*/
}

void ModelObject::Initialize(Fngine* fngine) {
	if (fngine) {
		SetFngine(fngine);
	}
	else {
		assert(0 && "Fngineのポインタがnullptrです");
	}
	InitializeData();
	worldTransform_.Initialize();
}

void ModelObject::Initialize(D3D12System& d3d12, const std::string& filename, const std::string& directoryPath) {
	InitializeResource(d3d12, filename, directoryPath);
	InitializeData();
	vertexResource_->SetName(L"vertexResource");
	wvpResource_->SetName(L"wvpResource");
	materialResource_->SetName(L"materialResource");

	worldTransform_.Initialize();
	uvTransform_.Initialize();
}

void ModelObject::Initialize(D3D12System& d3d12, ModelData& modelData) {
	InitializeResource(d3d12,modelData);
	InitializeData();
}

//==========-+-==========
// Draw Function
//==========-+-==========

void ModelObject::Draw() {
	fngine_->GetCommand().GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawBase();
	fngine_->GetCommand().GetList().GetList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

void ModelObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawBase(command, pso, light, tex);
	command.GetList().GetList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

void ModelObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, D3D12_GPU_DESCRIPTOR_HANDLE& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawBase(command, pso, light, tex);
	command.GetList().GetList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

//==========-+-==========
// Other Function
//==========-+-==========

void ModelObject::SetWVPData(Matrix4x4 WVP) {
	wvpData_->WVP = WVP;
	wvpData_->World = worldTransform_.mat_;
	materialData_->uvTransform = uvTransform_.mat_;
}

void ModelObject::LocalToWorld() {
	worldTransform_.LocalToWorld();
	uvTransform_.LocalToWorld();
}

ModelData ModelObject::LoadObjFile(const std::string& filename, const std::string& directoryPath) {
	/////////////////
	// 変数宣言
	/////////////////
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	/////////////////
	// ファイルをひらく
	/////////////////
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	/////////////////
	// ModelDataを構築する
	/////////////////
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		// 頂点位置
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		// 頂点テクスチャ座標
		else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		// 頂点法線
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		// 面
		else if (identifier == "f")
		{
			// 1行分の頂点定義をすべて取得
			std::vector<std::string> vertexDefs;
			std::string vertexDefinition;
			while (s >> vertexDefinition)
			{
				vertexDefs.push_back(vertexDefinition);
			}

			// 3頂点未満は無視
			if (vertexDefs.size() < 3) continue;

			// 扇形分割で三角形を生成
			for (size_t i = 1; i + 1 < vertexDefs.size(); ++i)
			{
				VertexData triangle[3];
				std::string vdefs[3] = { vertexDefs[0], vertexDefs[i], vertexDefs[i + 1] };
				for (int faceVertex = 0; faceVertex < 3; ++faceVertex)
				{
					std::istringstream v(vdefs[faceVertex]);
					uint32_t elementIndices[3] = {};
					/*for (int element = 0; element < 3; ++element)
					{
						std::string index;
						std::getline(v, index, '/');
						elementIndices[element] = std::stoi(index);
					}*/
					for (int element = 0; element < 3; ++element) {
						std::string index;
						if (!std::getline(v, index, '/')) {
							elementIndices[element] = 0; // デフォルト値 or エラー処理
						}
						else if (index.empty()) {
							elementIndices[element] = 0; // デフォルト値 or エラー処理
						}
						else {
							elementIndices[element] = std::stoi(index);
						}
					}
					Vector4 position = positions[elementIndices[0] - 1];
					//Vector2 texcoord = texcoords[elementIndices[1] - 1];
					Vector2 texcoord;
					if (elementIndices[1] > 0 && elementIndices[1] <= texcoords.size()) {
						texcoord = texcoords[elementIndices[1] - 1];
					}
					else {
						texcoord = { 0.0f, 0.0f }; // デフォルトUV
					}
					Vector3 normal = normals[elementIndices[2] - 1];
					triangle[faceVertex] = { position, texcoord, normal };
				}
				// 頂点の順序を逆にして追加（右手系→左手系変換のため）
				modelData.vertices.push_back(triangle[2]);
				modelData.vertices.push_back(triangle[1]);
				modelData.vertices.push_back(triangle[0]);
			}
		}

		// mtllib
		else if (identifier == "mtllib")
		{
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にmtlはobjファイルと同一階層に配置指せるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}


	/////////////////
	// 構築したModelDataをreturnする
	/////////////////
	return modelData;
}

// ----------------------------------------------------- [ PRIVATE ] -------------------------------------------------- //
void ModelObject::InitializeResource(D3D12System& d3d12, const std::string& filename, const std::string& directoryPath) {
	modelData_ = LoadObjFile(filename, directoryPath);
	vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * modelData_.vertices.size());
	materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));
}

void ModelObject::InitializeResource(D3D12System& d3d12, ModelData& modelData) {
	modelData_ = modelData;
	vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * modelData_.vertices.size());
	materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));
}

void ModelObject::InitializeData() {
	InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), true);
	InitializeWVPD();

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

MaterialData ModelObject::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 1, 中で必要となる変数の宣言
	MaterialData materialData;// 構築するMaterialData
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	// 2, ファイルを開く
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 行の最初の文字列を取得
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;// テクスチャファイル名を取得
			// テクスチャファイルのパスを構築
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}


	// 3, 実際にファイルを読み、MaterialDataを構築していく
	// 4, MaterialDataを返す
	return materialData;
}
