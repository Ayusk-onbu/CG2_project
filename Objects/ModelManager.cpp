#include "ModelManager.h"
#include <sstream>
#include "Log.h"

//void ModelManager::LoadObjFiles(const std::vector<std::string>& filenames, const std::string& directoryPath) {
//	
//	for (std::string file : filenames) {
//		// もし、すでに読み込んでいるファイルパスがあれば、読み込まない
//		if (pathToIndex_.find(file) != pathToIndex_.end()) {
//			continue;
//		}
//		// ファイルを読み込み、ModelDataを構築する
//		ModelData modelData = LoadObjFile(file, directoryPath);
//
//		// ModelDataのハッシュ値を計算する
//		std::string modelHash = HashModelData(modelData);
//		// もし、同じハッシュ値のModelDataがあれば、読み込まない
//		if (modelHashToIndex.find(modelHash) != modelHashToIndex.end()) {
//			// すでに同じModelDataが存在するので、インデックスを取得して保存
//			pathToIndex_[file] = modelHashToIndex[modelHash];
//			Log::ViewFile("ModelData already exists for file: " + file + ", using existing index: " + std::to_string(modelHashToIndex[modelHash]));
//			continue;
//		}
//
//		// ModelDataを保存する
//		modelData_.push_back(modelData);
//		int newIndex = static_cast<int>(modelData_.size() - 1);
//
//		modelHashToIndex[modelHash] = newIndex;
//		// ファイルパスとインデックスを保存する
//		pathToIndex_[file] = newIndex;
//
//	}
//}
//
//ModelData ModelManager::LoadObjFile(const std::string& filename, const std::string& directoryPath) {
//	/////////////////
//	// 変数宣言
//	/////////////////
//	ModelData modelData;
//	std::vector<Vector4> positions;
//	std::vector<Vector3> normals;
//	std::vector<Vector2> texcoords;
//	std::string line;
//
//	/////////////////
//	// ファイルをひらく
//	/////////////////
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//
//	/////////////////
//	// ModelDataを構築する
//	/////////////////
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//		// 頂点位置
//		if (identifier == "v")
//		{
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.x *= -1.0f;
//			position.w = 1.0f;
//			positions.push_back(position);
//		}
//		// 頂点テクスチャ座標
//		else if (identifier == "vt")
//		{
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoord.y = 1.0f - texcoord.y;
//			texcoords.push_back(texcoord);
//		}
//		// 頂点法線
//		else if (identifier == "vn")
//		{
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1.0f;
//			normals.push_back(normal);
//		}
//		// 面
//		else if (identifier == "f")
//		{
//			// 1行分の頂点定義をすべて取得
//			std::vector<std::string> vertexDefs;
//			std::string vertexDefinition;
//			while (s >> vertexDefinition)
//			{
//				vertexDefs.push_back(vertexDefinition);
//			}
//
//			// 3頂点未満は無視
//			if (vertexDefs.size() < 3) continue;
//
//			// 扇形分割で三角形を生成
//			for (size_t i = 1; i + 1 < vertexDefs.size(); ++i)
//			{
//				VertexData triangle[3];
//				std::string vdefs[3] = { vertexDefs[0], vertexDefs[i], vertexDefs[i + 1] };
//				for (int faceVertex = 0; faceVertex < 3; ++faceVertex)
//				{
//					std::istringstream v(vdefs[faceVertex]);
//					uint32_t elementIndices[3] = {};
//					for (int element = 0; element < 3; ++element)
//					{
//						std::string index;
//						std::getline(v, index, '/');
//						elementIndices[element] = std::stoi(index);
//					}
//					Vector4 position = positions[elementIndices[0] - 1];
//					Vector2 texcoord = texcoords[elementIndices[1] - 1];
//					Vector3 normal = normals[elementIndices[2] - 1];
//					triangle[faceVertex] = { position, texcoord, normal };
//				}
//				// 頂点の順序を逆にして追加（右手系→左手系変換のため）
//				modelData.vertices.push_back(triangle[2]);
//				modelData.vertices.push_back(triangle[1]);
//				modelData.vertices.push_back(triangle[0]);
//			}
//		}
//
//		// mtllib
//		else if (identifier == "mtllib")
//		{
//			// materialTemplateLibraryファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			// 基本的にmtlはobjファイルと同一階層に配置指せるので、ディレクトリ名とファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//
//
//	/////////////////
//	// 構築したModelDataをreturnする
//	/////////////////
//	return modelData;
//}
//
//
//MaterialData ModelManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	// 1, 中で必要となる変数の宣言
//	MaterialData materialData;// 構築するMaterialData
//	std::string line;
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//	// 2, ファイルを開く
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;// 行の最初の文字列を取得
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;// テクスチャファイル名を取得
//			// テクスチャファイルのパスを構築
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//
//
//	// 3, 実際にファイルを読み、MaterialDataを構築していく
//	// 4, MaterialDataを返す
//	return materialData;
//}
//
//std::string ModelManager::HashModelData(const ModelData& model) {
//	std::ostringstream oss;
//	for (const auto& v : model.vertices) {
//		oss << v.position.x << v.position.y << v.position.z
//			<< v.texcoord.x << v.texcoord.y
//			<< v.normal.x << v.normal.y << v.normal.z;
//	}
//	oss << model.material.textureFilePath;
//	return std::to_string(std::hash<std::string>{}(oss.str()));
//}

/*
* void LoadObjFile(何個でもファイルパスを書ける(例){a.obj,b.obj,c.obj}){
* 書いたファイルパスを順番に読み込んで、ModelDataを構築していく
* * もし、同じファイルパスがあった場合は、読み込まない
* ただし、ファイルパスは保存し、同じ情報を指すようにする。
* 同じものがあったと分かるように報告もする

* }
* 
* 
*/