#include "ModelManager.h"
#include <sstream>
#include "Log.h"
#include "Trigonometric.h"

void ModelManager::Initialize(Fngine* fngine) {
	pFngine_ = fngine;
}


//   =====================
// 【 ModelData保存系関数 】
//   =====================

std::string ModelManager::LoadObj(const std::string& filename, const std::string& directoryPath, LoadFileType type) {
	// model を生成
	std::unique_ptr<ModelObject>model = std::make_unique<ModelObject>();
	// Model 初期化 -この処理はObjファイルのみになってしまっている
	model->Initialize(pFngine_->GetD3D12System(),filename, directoryPath,type);
	// Model Name を生成
	std::string modelName = filename;
	// ファイルの名前から.以降の拡張子を削除
	remove_extension_in_place(modelName);
	
	// -------------------------------------------------------------
	// モデルの物理三角形を抽出し、静的BVHを構築・保存する
	// -------------------------------------------------------------
	const auto& modelData = model->GetModelData();
	auto triangles = ExtractPhysicsTriangles(modelData.vertices, modelData.indices, Matrix4x4::Make::Identity());
	if (!triangles.empty()) {
		auto bvh = std::make_unique<BVH>();
		bvh->Build(triangles);
		bvhs_.emplace(modelName, std::move(bvh));
		Log::View("BVH Built successfully for Model: " + modelName);
	}
	
	// 追加
	models_.emplace(modelName, std::move(model));
	// 返す
	return modelName;
}

void ModelManager::AddObject(const std::string& ID, const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices) {
	auto it = objects_.find(ID);
	if (it == objects_.end()) {
		Log::View("The : " + ID + "Object -> Not already Exists");
		std::unique_ptr<ObjectData>object = std::make_unique<ObjectData>();
		object->MakeObjectData(pFngine_, vertices, indices);
		objects_.emplace(ID, std::move(object));

		// -------------------------------------------------------------
		// AddObject で追加された形状からも BVH を構築して保存する
		// -------------------------------------------------------------
		auto triangles = ExtractPhysicsTriangles(vertices, indices, Matrix4x4::Make::Identity());
		if (!triangles.empty()) {
			auto bvh = std::make_unique<BVH>();
			bvh->Build(triangles);
			bvhs_.emplace(ID, std::move(bvh)); // IDをキーにして保存
			Log::View("BVH Built successfully for Object: " + ID);
		}
	}
	else {
		Log::View("The : " + ID + "Object -> Already Exists");
		return;
	}
}

//   ==========================
// 【 データの海から取得系関数 】
//   ==========================

ModelData& ModelManager::LoadModelData(const std::string& ID) {
	auto it = models_.find(ID);
	if (it == models_.end()) {
		Log::ViewFile("Not Found Model Data\n");
	}
	return it->second->GetModelData();
}

ObjectData& ModelManager::LoadObjectData(const std::string& ID) {
	auto it = objects_.find(ID);
	if (it == objects_.end()) {
		Log::ViewFile("Not Found Model Data\n");
		return *objects_.begin()->second;
	}
	return *it->second;
}

// BVH取得関数
const BVH* ModelManager::GetBVH(const std::string& ID) const {
	auto it = bvhs_.find(ID);
	if (it != bvhs_.end()) {
		return it->second.get();
	}
	Log::View("\nNot Found BVH Data for ID: " + ID);
	return nullptr;
}


//   ===============
// 【 BVH生成系関数 】
//   ===============

std::vector<PhysicsTriangle> ModelManager::ExtractPhysicsTriangles(const std::vector<VertexData>& verticesData, const std::vector<uint32_t>& indicesData, const Matrix4x4& matData) {
	// ポリゴンの情報
	std::vector<PhysicsTriangle> triangles;
	// 頂点情報
	const auto& vertices = verticesData;
	const auto& indices = indicesData;
	// ワールド行列
	const Matrix4x4& worldMat = matData;

	// 1. IndexBufferがある場合（インデックス付きメッシュ）
	if (!indices.empty()) {
		for (size_t i = 0; i < indices.size(); i += 3) {
			PhysicsTriangle tri;

			// 計算負荷を下げるため、Transform（行列の掛け算）は1頂点につき1回だけ行う
			auto pos0 = Matrix4x4::Transform(worldMat, vertices[indices[i]].position);
			auto pos1 = Matrix4x4::Transform(worldMat, vertices[indices[i + 1]].position);
			auto pos2 = Matrix4x4::Transform(worldMat, vertices[indices[i + 2]].position);

			tri.v0 = { pos0.x, pos0.y, pos0.z };
			tri.v1 = { pos1.x, pos1.y, pos1.z };
			tri.v2 = { pos2.x, pos2.y, pos2.z };

			// ポリゴンに番号を振る
			tri.originalIndex = static_cast<int>(i / 3);

			triangles.push_back(tri);
		}
	}
	// 2. IndexBufferがない場合（頂点が直接並んでいるトライアングルスープ）
	else if (!vertices.empty()) {
		// 頂点が3つずつのセットになっている前提でループを回す
		for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
			PhysicsTriangle tri;

			auto pos0 = Matrix4x4::Transform(worldMat, vertices[i].position);
			auto pos1 = Matrix4x4::Transform(worldMat, vertices[i + 1].position);
			auto pos2 = Matrix4x4::Transform(worldMat, vertices[i + 2].position);

			tri.v0 = { pos0.x, pos0.y, pos0.z };
			tri.v1 = { pos1.x, pos1.y, pos1.z };
			tri.v2 = { pos2.x, pos2.y, pos2.z };

			// ポリゴンに番号を振る
			tri.originalIndex = static_cast<int>(i / 3);

			triangles.push_back(tri);
		}
	}

	// 最後にまとめて返す（これでif文の条件から外れても安全）
	return triangles;
}