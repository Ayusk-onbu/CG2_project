#pragma once
#include "Structures.h"
#include <vector>

namespace GuideCurve {
	
	// 制御点の情報
	struct ControllerPoint {
		Vector3 position;     // 座標
		float radius;         // 髪の太さ

		Vector3 color;        // 色
		float nextToLength;   // 次のポイントへの長さ

		Vector3 homePosition; // 最初の固定値
		float physicsWeight;  // 物理演算影響率(根元 0.0 ～ 毛先 1.0)
	};
	
	// 物理演算のための情報
	struct  HairPhysicsConfig {
		float stiffness;      // 剛性
		float restoringForce; // 復元力
		float damping;        // 減衰力
		float padding;        // カス
	};

	struct FrameConfig
	{
		Vector3 gravity; // 重力ベクトル (例: 0, -9.8, 0)
		float deltaTime; // 前フレームからの経過時間
		Vector3 windDirection; // 風の向きと強さ
		float time; // 経過時間（風のうねり用）
		Vector3 moveDirection;// 動いた方向
		float padding;
	};

	// 制御点の集合体
	struct GuideHear {
		std::vector<ControllerPoint>points;
	};

	// ガイドの集合体
	struct Main {
		std::vector<GuideHear>guides;
	};
}

namespace Strands {
	// 生成後のdata情報
	struct StrandVertex {
		Vector3 position;
		float radius;
		Vector3 color;
		float padding;
	};

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
		uint32_t pointPerGuide;  // ガイド一本当たりの制御点数
		uint32_t pointPerStrand; // 子ストランド一本当たりの頂点数
		float numStrands;        // 髪の総本数
	};

	struct HairMakeConfig {
		float globalSpreadRadius; // 全体の広がり
		float globalHairThickness; // 全体の太さ倍率
		float paddings[2];
	};

	struct Strand {
		ChildStrand bindData;
		std::vector<StrandVertex> vertices;
	};

	std::vector<Strand> GenerateStrandOneGuide(
		const GuideCurve::GuideHear& guide,
		int numStrands,
		float spreadRadius);

	std::vector<StrandVertex> FlattenStrands(const std::vector<Strand>& strands);
}