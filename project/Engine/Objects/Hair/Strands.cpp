#include "GuideCurve.h"
#include "RandomUtils.h"

namespace Strands {
	std::vector<Strand> GenerateStrandOneGuide(
		const GuideCurve::GuideHear& guide,
		int numStrands,
		float spreadRadius)
	{
		std::vector<Strand> generatedStrands;

		for (int s = 0; s < numStrands; ++s) {
			Strand strand;

			// バインドデータの決定
			strand.bindData.parentGuideIds[0] = 0;// 一本のガイド
			strand.bindData.weights[0] = 1.0f;// 影響度100%

			auto rand = RandomUtils::GetInstance();

			// 円に配置する座標を計算
			float angle = static_cast<float>(rand->GetHighRandom().GetInt(0, 360));// 0 ～ 360度を取得
			float r = sqrtf(rand->GetHighRandom().GetFloat(0.0f, 1.0f)) * spreadRadius;
			strand.bindData.offset = Vector2(std::cosf(Deg2Rad(angle)) * r, std::sinf(Deg2Rad(angle)) * r);

			// 長さもランダム
			strand.bindData.lengthScale = 0.5f + rand->GetHighRandom().GetFloat(0.0f, 1.0f) * 0.8f;

			// 頂点の計算
			int numPoints = static_cast<int>(guide.points.size());
			for (int i = 0; i < numPoints; ++i) {
				const auto& gPoint = guide.points[i];
				StrandVertex sVertex;

				// 垂直な平面(ローカルの計算)
				Vector3 tangent;
				if (i < numPoints - 1) {
					tangent = Normalize(guide.points[i + 1].position - gPoint.position);
				}
				else {
					tangent = Normalize(gPoint.position - guide.points[i - 1].position);
				}

				// 接線に垂直なベクトルを作成
				Vector3 arbitraryUp = Vector3(0.0f, 1.0f, 0.0f);
				if (std::abs(Dot(tangent, arbitraryUp)) > 0.99f) {
					// 接戦がほぼ↑ならエラーが出るので、別ベクトルに変更
					arbitraryUp = Vector3(1.0f, 0.0f, 0.0f);
				}
				// X軸の向き
				Vector3 normal = Normalize(CrossProduct(tangent, arbitraryUp));
				// Y軸の向き
				Vector3 binormal = CrossProduct(tangent, normal);

				// オフセットの適用 2D -> 3D
				Vector3 offset3D = (normal * strand.bindData.offset.x) +
								(binormal * strand.bindData.offset.y);

				sVertex.position = gPoint.position + offset3D;

				// 毛先の計算
				float tipTaper = 1.0f - ((float)i / (numPoints) / 2.0f);
				sVertex.radius = gPoint.radius * tipTaper;

				sVertex.color = gPoint.color;

				strand.vertices.push_back(sVertex);
			}

			generatedStrands.push_back(strand);
		}

		return generatedStrands;
	}

	std::vector<StrandVertex> FlattenStrands(const std::vector<Strand>& strands) {
		std::vector<StrandVertex> flatVertices;

		// 必要なメモリの確保
		size_t totalVertices = 0;
		for (const auto& strand : strands) {
			totalVertices += strand.vertices.size();
		}
		flatVertices.reserve(totalVertices);

		// データを一つにする
		for (const auto& strand : strands) {
			for (const auto& vertex : strand.vertices) {
				flatVertices.push_back(vertex);
			}
		}

		return flatVertices;
	}
}

/*
* Structured<Strands::StrandVertex> hairVertexBuffer(p_fngine);
hairVertexBuffer.Initialize(50);
*/