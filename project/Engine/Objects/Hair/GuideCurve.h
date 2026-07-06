#pragma once
#include "Structures.h"
#include <vector>

namespace GuideCurve {
	
	// --------------------------------------------------
	// 【保存するデータ】
	// --------------------------------------------------

	// 制御点の情報
	struct ControllerPoint {
		Vector3 position;   // 座標 
		float radius;// 髪の太さ
		Vector3 color;// 色
		float nextToLength;// 次のポイントへの長さ
		Vector3 homePosition;// 最初の固定値
		float physicsWeight;// 物理演算影響率(根元 0.0 ～ 毛先 1.0)
	};
	
	// 物理演算のための髪の情報
	struct  HairPhysicsConfig {
		float stiffness;// 剛性
		float restoringForce;// 復元力
		float damping;// 減衰力
		float padding;// カス
	};

	struct GuideInfo {
		uint32_t vertexStartIndex;
		uint32_t vertexCount;
	};

	// 制御点の集合体
	struct GuideHear {
		std::vector<ControllerPoint>points;
	};

	// --------------------------------------------------
	// 【実行時のみのデータ】
	// --------------------------------------------------

	struct FrameConfig
	{
		// 重力ベクトル (例: 0, -9.8, 0)
		Vector3 gravity;
		// 前フレームからの経過時間
		float deltaTime; 
		// 風の向きと強さ
		Vector3 windDirection;
		// 経過時間（風のうねり用）
		float time; 
		// 動いた方向
		Vector3 moveDirection;
		// カス
		float padding;
	};

	struct Main {
		std::vector<GuideHear>guides;
	};// うーん
}

namespace Strands {
	// --------------------------------------------------
	// 【保存するデータ】
	// --------------------------------------------------

	// ガイドからストランドを生成するdata情報
	struct ChildStrand {
		uint32_t parentGuideIds[3]; //☆ 影響を受ける親ガイドの情報
		uint32_t blendMode;         //☆ 0 : 単一 1 : 複数ブレンド

		float weights[3];           //☆ 影響力
		float lengthScale;          //☆ 髪の倍率

		Vector2 offset;             //☆ ガイドからのオフセット位置
		float twistAngle;           // ねじれの大きさ
		float clumpForce;           // 束感の強さ

		uint32_t seed;              // 乱数シード
		float waveAmplitude;        // うねりの強さ
		float waveFrequency;        // うねりの細かさ
		float noise;                // アホ毛
	};

	struct HairConfig {
		uint32_t numGuides;      // 親ガイドの総数
		uint32_t pointPerGuide;  // ガイド一本当たりの制御点数：使わないかも
		uint32_t pointPerStrand; // 子ストランド一本当たりの頂点数：使わないかも
		float numStrands;        // 髪の総本数
	};// 計算するほうがだるい

	// 髪の見た目を調整するデータ
	struct HairMakeConfig {
		float globalSpreadRadius; // 全体の広がり
		float globalHairThickness; // 全体の太さ倍率
		float paddings[2];
	};

	// --------------------------------------------------
	// 【実行時(GPU用)に動的生成するデータ】
	// --------------------------------------------------

	// 生成後のdata情報
	struct StrandVertex {
		Vector3 position;
		float radius;
		Vector3 color;
		float padding;
	};

	struct StrandInfo {
		uint32_t vertexStartIndex; // この髪の頂点が始まるインデックス
		uint32_t vertexCount;      // この髪の頂点数
		uint32_t aabbStartIndex;   // この髪のAABB（およびSegmentData）が始まるインデックス
	};// Guideデータをロード時、もしくはガイド生成時に生成

	struct SegmentData {
		uint32_t v0_Index; // 始点頂点のインデックス
		uint32_t v1_Index; // 終点頂点のインデックス
	};// セグメントのデータはStrandInfoで良くないか？

	struct Strand {
		ChildStrand bindData;
		std::vector<StrandVertex> vertices;
	};// Loadデータから作成する

	// ==================================================
	// セーブ/ロード用のアセットルートデータ
	// ==================================================
	struct HairSaveData {
		GuideCurve::HairPhysicsConfig physicsConfig;
		HairMakeConfig makeConfig;
		HairConfig hairConfig;

		std::vector<GuideCurve::ControllerPoint>points;       // ガイドのリスト
		std::vector<GuideCurve::GuideInfo>guideInfo;

		std::vector<ChildStrand> childStrands;           // 生成された子髪の設定リスト
		std::vector<Strands::StrandInfo> strandInfos;       // 生成された子髪の情報リスト

		std::vector<SegmentData> segments;
	};

	std::vector<Strand> GenerateStrandOneGuide(
		const GuideCurve::GuideHear& guide,
		int numStrands,
		float spreadRadius);

	std::vector<StrandVertex> FlattenStrands(const std::vector<Strand>& strands);
}