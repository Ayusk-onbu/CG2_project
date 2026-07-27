#pragma once
#include "ModelObject.h"
#include "BVH.h"
#include "ISingleton.h"
#include <unordered_map>


class ModelManager : public ISingleton<ModelManager>
{
public:
	friend class ISingleton<ModelManager>;

public:
	void Initialize(Fngine* fngine);

	/// <summary>
	/// データを送る
	/// </summary>
	/// <param name="ID"></param>
	/// <returns></returns>
	ModelData& LoadModelData(const std::string& ID);

	/// <summary>
	/// IDに対応したBVHを取得する
	/// </summary>
	/// <param name="ID">Modelの名前</param>
	/// <returns>メッシュを細かくしたデータ</returns>
	const BVH* GetBVH(const std::string& ID) const;

	/// <summary>
	/// ファイルからModelデータをロードする
	/// </summary>
	/// <param name="filename">ファイルネーム</param>
	/// <param name="directoryPath"></param>
	/// <param name="type">三角面化してないならType::OBJ</param>
	/// <returns></returns>
	std::string LoadObj(const std::string& filename, const std::string& directoryPath = "resources",LoadFileType type = LoadFileType::Assimp);
private:
	/// <summary>
	/// 頂点dataやIndexDataから三角形のメッシュを作り出す
	/// </summary>
	/// <returns></returns>
	std::vector<PhysicsTriangle>ExtractPhysicsTriangles(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices, const Matrix4x4& mat);

	uint32_t modelCount_;
	Fngine* pFngine_;

public:
	ObjectData& LoadObjectData(const std::string& ID);

	void AddObject(const std::string& ID,const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

private:
	// 図鑑的な存在
	// [ 最初ModelDataだけでいいかと思ったが、表示したいかもだし別にいいかという判断。ただ、オブジェクトプールしたいから将来的に変更の可能性 ]
	std::unordered_map<std::string, std::unique_ptr<ModelObject>>models_;

	// 名前：ModelData
	std::unordered_map<std::string, std::unique_ptr<ObjectData>>objects_;

	// 名前：BVH
	std::unordered_map<std::string, std::unique_ptr<BVH>> bvhs_;
};

