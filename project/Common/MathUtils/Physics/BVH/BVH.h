#pragma once
#include "AABB.h"
#include "PhysicsTriangle.h"

#include <memory>
#include <vector>

// ================================================
// 【 BVH 】 とは
// 名称：Bounding Volume Hierarchy
// 読み方：バウンディング・ボリューム・ヒエラルキー
// 意味：境界ボリューム階層
// ================================================

struct BVHNode {
	AABB bounds;
	// 自身を分けたときの左側
	std::unique_ptr<BVHNode> left = nullptr;
	// 自身を分けたときの右側
	std::unique_ptr<BVHNode> right = nullptr;
	// 自身が葉ノードの場合、含まれるIndexを登録
	std::vector<int> triangleIndices;
	// 自身が葉ノードかどうか
	bool IsLeaf()const {
		return left == nullptr && right == nullptr;
	}
};

// GPUへ送るBVHノード構造体
struct GPUBVHNode {
	Vector3 minBounds;
	int leftChildIndex;  // -1 なら葉ノード（Leaf）

	Vector3 maxBounds;
	int rightChildIndex; // 葉ノードの場合、この値は「polygonStartIndex」として使う

	int polygonCount;    // 葉ノードに含まれるポリゴン数（0なら内部ノード）
	int padding[3];      // 16バイトアライメント用のパディング
};

// GPUへ送るポリゴン構造体
struct GPUPolygon {
	Vector3 v0;
	float pad0;
	Vector3 v1;
	float pad1;
	Vector3 v2;
	float pad2;
};

// BVHを管理する
class BVH {
public:
	// 三角形(ポリゴン)のリストからツリーを構築
	void Build(std::vector<PhysicsTriangle>& triangles);
	
	// ルートノードの取得
	const BVHNode* GetRoot()const { return root_.get();}
	
	/// <summary>
	/// 設定したAABBと当たっていたらその中にある三角形のIndexを返す
	/// </summary>
	/// <param name="node"></param>
	/// <param name="targetAABB">：当たり判定を取りたい物体のAABB</param>
	/// <param name="outIndices">当たり判定に使う三角形のIndexの情報</param>
	void CollectPotentialTriangles(const BVHNode* node, const AABB& targetAABB, std::vector<int>& outIndices) const;
	
	// プレイヤーのAABBと重なる可能性のある三角形をすべて抽出する
	void QueryTriangles(const AABB& targetAABB, std::vector<PhysicsTriangle>& outTriangles) const;

	/// <summary>
	/// GPUへ転送するために、ポインタツリーを1次元の配列構造へ変換する
	/// </summary>
	void Flatten(std::vector<GPUBVHNode>& outNodes, std::vector<GPUPolygon>& outPolygons) const;
private:
	// 再帰的にノードを構築(Buildで使用)
	std::unique_ptr<BVHNode> BuildRecursive(std::vector<PhysicsTriangle>& triangle, size_t start, size_t end);
	
	// 再帰的にノードを辿る
	void QueryRecursive(const BVHNode* node, const AABB& targetAABB, std::vector<PhysicsTriangle>& outTriangles) const;

	int FlattenRecursive(const BVHNode* node, std::vector<GPUBVHNode>& outNodes, std::vector<GPUPolygon>& outPolygons) const;

private:
	// 構築したBVHの情報
	std::unique_ptr<BVHNode> root_ = nullptr;
	
	// BVHの構築に使用したTriangleの情報
	std::vector<PhysicsTriangle>allTriangles_;
};