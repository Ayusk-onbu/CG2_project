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
    Sphere      // 3Dの球体
};

// ---------------------------------------------------------
// エミッターの生成パラメータをまとめた構造体
// ---------------------------------------------------------
struct EmitterConfig {
    // 発生させる範囲の形状
    EmitterShapeType shapeType = EmitterShapeType::Rectangle;

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
    std::vector<Vector3> GeneratePositions(const EmitterConfig& config, uint32_t count);

private:
    // 各形状の計算ロジック（前述のアルゴリズムをここに実装）
    Vector3 CalculateRectangle(const EmitterConfig& config);

    Vector3 CalculateCircle(const EmitterConfig& config);

    Vector3 CalculateTriangle(const EmitterConfig& config);

    Vector3 CalculateCube(const EmitterConfig& config);
    
    Vector3 CalculateSphere(const EmitterConfig& config);

    // 3Dに戻して返す
    Vector3 ApplyTransform(const Vector3& localPos, const EmitterConfig& config);
};

// ほしい機能
// 初期速度も返す
// VolumeかSurface