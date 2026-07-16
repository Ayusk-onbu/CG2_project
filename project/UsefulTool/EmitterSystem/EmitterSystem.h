#pragma once
#include "Structures.h"
#include "../ISingleton.h"
#include <vector>

// ---------------------------------------------------------
// エミッターの形状の種類を定義する列挙型
// ---------------------------------------------------------
enum class EmitterShapeType {
    Rectangle,  // 2D平面の四角形
    Triangle,   // 2D平面の三角形
    Circle,     // 2D平面の円
    Cube,       // 3Dの立方体
    Sphere,     // 3Dの球体
    Noise,      //
};

enum class EmitSpace {
    Volume, // 領域の内部に発生させる
    Surface,// 領域の表面のみに発生させる
};

// ---------------------------------------------------------
// エミッターの生成パラメータをまとめた構造体
// ---------------------------------------------------------
struct EmitterConfig {
    // 発生させる範囲の形状
    EmitterShapeType shapeType = EmitterShapeType::Rectangle;
    // 発生させる領域
    EmitSpace space = EmitSpace::Volume;

    // エミッター自体のワールド空間における基本設定
    Vector3 position = { 0.0f, 0.0f, 0.0f }; // 配置位置（中心座標）
    Vector3 rotation = { 0.0f, 0.0f, 0.0f }; // 向き（オイラー角などで指定）

    // 形状ごとのサイズパラメータ
    // - Rectangle / Cube の場合： 各軸(x, y, z)の幅・高さ・奥行き
    // - Circle / Sphere の場合 ： x を「半径」として扱う
    Vector3 size = { 1.0f, 1.0f, 1.0f };

    // Triangle（三角形）を指定した場合のみ使用する、3つのローカル頂点座標
    Vector3 vA = { 0.0f,  1.0f, 0.0f };
    Vector3 vB = { 1.0f, -1.0f, 0.0f };
    Vector3 vC = { -1.0f, -1.0f, 0.0f };

    // 初速の速さ
    float initialSpeed = 5.0f;

    std::string noiseTextureName = ""; // 使用するノイズテクスチャのキー
    float noiseThreshold = 0.5f;       // 発生を許可する明るさの閾値 (0.0 ～ 1.0)
};

// 戻り値用の構造体
struct EmissionResult {
    Vector3 position; // 発生座標
    Vector3 velocity; // 初期速度（方向 * スピード）
};

class EmitterSystem : 
    public ISingleton<EmitterSystem>
{
public:
    friend class ISingleton<EmitterSystem>;
public:
    EmitterSystem() {}

    // 指定したパラメータに基づいて、ランダムなワールド座標を生成して一括で返す
    // @param config : エミッターの形状や位置、サイズなどの設定
    // @param count  : 生成したい座標の数
    // @return       : 生成されたワールド座標のリスト
    std::vector<EmissionResult> GeneratePositions(const EmitterConfig& config, uint32_t count);

private:
    // 各形状の計算ロジック（前述のアルゴリズムをここに実装）
    EmissionResult CalculateRectangle(const EmitterConfig& config);
    EmissionResult CalculateCircle(const EmitterConfig& config);
    EmissionResult CalculateTriangle(const EmitterConfig& config);
    EmissionResult CalculateCube(const EmitterConfig& config);
    EmissionResult CalculateSphere(const EmitterConfig& config);
    EmissionResult CalculateNoise(const EmitterConfig& config);

    // 3Dに戻して返す
    EmissionResult ApplyTransform(const EmissionResult& localRes, const EmitterConfig& config);
};