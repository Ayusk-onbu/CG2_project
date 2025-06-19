#include "ModelObject.h"
#include <sstream>

void ModelObject::Initialize(D3D12System d3d12, const std::string& filename, const std::string& directoryPath) {
	InitializeResource(d3d12, filename, directoryPath);
	InitializeData();
}

void ModelObject::Draw(TheOrderCommand& command, PSO& pso, DirectionLight& light, Texture& tex) {
	command.GetList().GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	object_.DrawBase(command, pso, light, tex);
	command.GetList().GetList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

void ModelObject::SetWVPData(Matrix4x4 WVP, Matrix4x4 world, Matrix4x4 uv) {
	object_.wvpData_->WVP = WVP;
	object_.wvpData_->World = world;
	object_.materialData_->uvTransform = uv;
}

void ModelObject::InitializeResource(D3D12System d3d12, const std::string& filename, const std::string& directoryPath) {

	modelData_ = LoadObjFile(filename, directoryPath);
	object_.vertexResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(VertexData) * modelData_.vertices.size());
	object_.materialResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(Material));
	object_.wvpResource_ = CreateBufferResource(d3d12.GetDevice().Get(), sizeof(TransformationMatrix));

}

void ModelObject::InitializeData() {
	object_.InitializeMD(Vector4(1.0f, 1.0f, 1.0f, 1.0f), true);
	object_.InitializeWVPD();

	object_.vertexBufferView_.BufferLocation = object_.vertexResource_->GetGPUVirtualAddress();
	object_.vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	object_.vertexBufferView_.StrideInBytes = sizeof(VertexData);

	object_.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&object_.vertexData_));
	std::memcpy(object_.vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

ModelData ModelObject::LoadObjFile(const std::string& filename, const std::string& directoryPath) {
	// 1, 中で必要となる変数の宣言
	ModelData modelData;// 構築するModelData
	std::vector<Vector4> positions;// v
	std::vector<Vector3> normals;// vn
	std::vector<Vector2>texcoords;// vt
	std::string line;// mtlの設定
	// 2, ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	// 3, 実装にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 行の最初の文字列を取得
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f; // X軸を反転
			//position.y *= -1.0f; // X軸を反転
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // Y軸を反転
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; // X軸を反転
			//normal.y *= -1.0f; // X軸を反転
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定
			for (int32_t faceVertex = 0; faceVertex < 3;++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは位置UV法線なので分解する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0;element < 3;++element) {
					std::string index;
					std::getline(v, index, '/');// '/'で分割
					elementIndices[element] = std::stoi(index);//文字列を数値に変換
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				// 頂点データを構築
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			// 頂点を逆順に登録
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にObjファイルと同一階層にMtlは存在
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	// 4, ModelDataを返す
	return modelData;
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