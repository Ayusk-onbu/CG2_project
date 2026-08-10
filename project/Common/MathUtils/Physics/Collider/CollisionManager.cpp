#include "CollisionManager.h"
#include "GJK.h"

void CollisionManager::Begin() {
	// Colliderの情報を初期化
	colliders_.clear();
}

void CollisionManager::SetColliders(Collider* collider) {
	// Colliderを登録
	if (collider) {
		colliders_.push_back(collider);
	}
}

/////////////////////////
///
///  メインシステム
///
/////////////////////////
void CollisionManager::CheckAllCollisions() {

	//   ======================================================================
	// 【 事前準備: 全動的なコライダーのAABBを最新の座標に合わせて更新しておく 】
	//   ======================================================================
	
	for (auto* col : colliders_) {
		col->UpdateAABB();
	}

	//   ================
	// 【 総当たりで判定 】
	//   ================
	for (size_t i = 0; i < colliders_.size(); ++i) {
		Collider* colA = colliders_[i];
		for (size_t j = i + 1; j < colliders_.size(); ++j) {

			Collider* colB = colliders_[j];

			// 万が一どちらかがnullptrならスキップ
			if (!colA || !colB) continue;

			// フィルターチェック (MyType と YourType のビット演算など)
			if (!CheckFilter(colA, colB)) continue;

			// ==========================================
			// 【第1段階】ブロードフェーズ (AABB判定)
			// ==========================================
			if (AABB::IsHitAABB2AABB(colA->GetAABB(), colB->GetAABB())) {
				// 押し出す量　※ 押し出す量を保存するため
				Vector3 pushOut = { 0.0f,0.0f,0.0f };

				// ==========================================
				// 【第2段階】ナローフェーズ (詳細判定)
				// ==========================================
				// AABB同士が重なっていたら、詳細な形状で判定する
				if (CheckNarrowPhase(colA, colB, pushOut))
				{
					// 最終的に当たっていたらコールバック呼び出し！
					colA->OnCollision(colB, -pushOut);
					colB->OnCollision(colA, pushOut);
				}
			}
		}
	}
}

//////////////////////
///
///  サポートシステム
///
//////////////////////
bool CollisionManager::CheckFilter(Collider* colliderA, Collider* colliderB) {
	return (colliderA->GetMyType() & colliderB->GetYourType()) != 0
		&& (colliderB->GetMyType() & colliderA->GetYourType()) != 0;
}

bool CollisionManager::CheckNarrowPhase(Collider* a, Collider* b, Vector3& outPush) {
	// ここで GetShapeType() を見て分岐する
	if (!a || !b) return false;

	// 両方が BVH を持っている場合 (メッシュ vs メッシュ)
	if (a->GetBVH() && b->GetBVH()) {
		return CheckBVHVsBVH(a, b, outPush);
	}

	// 片方だけが BVH を持っている場合 (メッシュ vs Convex / 単一ポリゴン)
	if (a->GetBVH()) {
		return CheckTriangleVsCollider(a, b, outPush);
	}
	if (b->GetBVH()) {
		Vector3 pushB;
		if (CheckTriangleVsCollider(b, a, pushB)) {
			outPush = -pushB; // 押し戻しベクトルの反転
			return true;
		}
		return false;
	}

	// どちらも BVH を持たない場合 (Convex vs Convex, 単一Triangle vs Convex など)
	// ※ 単一の TriangleCollider は Convex の一種なのでそのまま GJK でOK！
	return GJK(a, b, outPush);
}

// GJKアルゴリズムの本体
bool CollisionManager::GJK(Collider* a, Collider* b, Vector3& outPush) {

	std::vector<Vector3> simplex;
	if (GJK::_3D::GJK_GetSimplex(*a, *b, simplex)) {

		GJK::_3D::Contact3D contact = GJK::_3D::EPA(*a, *b, simplex);
		if (contact.hasCollision) {
			outPush = contact.normal * (contact.depth + 0.0001f);
			return true;
		}
	}
	return false; // 当たっていない
}

////////////////////////////////////////////////////////////////
// メッシュ (triCol) vs Convex (otherCol)
////////////////////////////////////////////////////////////////
bool CollisionManager::CheckTriangleVsCollider(Collider* triCol, Collider* otherCol, Vector3& outPush) {
	const BVH* bvh = triCol->GetBVH();
	if (bvh) {
		Matrix4x4 worldMat = triCol->GetWorldMatrix();
		Matrix4x4 invWorldMat = Matrix4x4::Inverse(worldMat);

		AABB localAABB = AABB::TransformAABB(otherCol->GetAABB(), invWorldMat);

		std::vector<PhysicsTriangle> localTriangles;
		bvh->QueryTriangles(localAABB, localTriangles);

		if (localTriangles.empty()) return false;

		bool hit = false;
		Vector3 bestPush{ 0, 0, 0 };

		for (const auto& localTri : localTriangles) {
			PhysicsTriangle worldTri = TransformTriangle(localTri, worldMat);

			if (!AABB::IsHitAABB2AABB(otherCol->GetAABB(), worldTri.GetAABB())) continue;

			TriangleCollider singleTriCol(worldTri);
			Vector3 localPush{ 0, 0, 0 };

			// ★ 修正ポイント: GJK(&singleTriCol, otherCol, localPush) の順で呼び出す
			// これにより localPush は 「singleTriCol (＝triCol) を押し戻すベクトル」 になる
			if (GJK(&singleTriCol, otherCol, localPush)) {
				hit = true;

				// 最も有効な（めり込みを正しく押し戻せる）ベクトルを選定
				if (LengthSquared(bestPush) == 0.0f || LengthSquared(localPush) > LengthSquared(bestPush)) {
					bestPush = localPush;
				}
			}
		}

		if (hit) {
			// GJK(&singleTriCol, otherCol) の結果なので、既に triCol 用の向きになっている
			outPush = bestPush;
		}
		return hit;
	}

	// 単一 TriangleCollider の場合
	return GJK(triCol, otherCol, outPush);
}

////////////////////////////////////////////////////////////////
// メッシュ (colA) vs メッシュ (colB)
////////////////////////////////////////////////////////////////
bool CollisionManager::CheckBVHVsBVH(Collider* colA, Collider* colB, Vector3& outPush) {
	const BVH* bvhA = colA->GetBVH();
	const BVH* bvhB = colB->GetBVH();
	if (!bvhA || !bvhB) return false;

	Matrix4x4 worldMatA = colA->GetWorldMatrix();
	Matrix4x4 invWorldMatA = Matrix4x4::Inverse(worldMatA);

	Matrix4x4 worldMatB = colB->GetWorldMatrix();
	Matrix4x4 invWorldMatB = Matrix4x4::Inverse(worldMatB);

	AABB localAABBForB = AABB::TransformAABB(colA->GetAABB(), invWorldMatB);
	std::vector<PhysicsTriangle> localTrianglesB;
	bvhB->QueryTriangles(localAABBForB, localTrianglesB);

	if (localTrianglesB.empty()) return false;

	bool hit = false;
	Vector3 bestPush{ 0, 0, 0 };

	for (const auto& localTriB : localTrianglesB) {
		PhysicsTriangle worldTriB = TransformTriangle(localTriB, worldMatB);

		AABB localAABBForA = AABB::TransformAABB(worldTriB.GetAABB(), invWorldMatA);
		std::vector<PhysicsTriangle> localTrianglesA;
		bvhA->QueryTriangles(localAABBForA, localTrianglesA);

		for (const auto& localTriA : localTrianglesA) {
			PhysicsTriangle worldTriA = TransformTriangle(localTriA, worldMatA);

			if (!AABB::IsHitAABB2AABB(worldTriA.GetAABB(), worldTriB.GetAABB())) continue;

			TriangleCollider singleTriA(worldTriA);
			TriangleCollider singleTriB(worldTriB);
			Vector3 localPush{ 0, 0, 0 };

			// ★ GJK(&singleTriA, &singleTriB) -> localPush は singleTriA (colA) 用のベクトル
			if (GJK(&singleTriA, &singleTriB, localPush)) {
				hit = true;
				if (LengthSquared(bestPush) == 0.0f || LengthSquared(localPush) > LengthSquared(bestPush)) {
					bestPush = localPush;
				}
			}
		}
	}

	if (hit) {
		outPush = bestPush;
	}
	return hit;
}