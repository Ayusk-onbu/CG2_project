#include "EmitterSystem.h"
#include "RandomUtils.h"

std::vector<Vector3> EmitterSystem::GeneratePositions(const EmitterConfig& config, uint32_t count) {
    std::vector<Vector3> results;
    results.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        Vector3 pos = {};

        switch (config.shapeType) {
        case EmitterShapeType::Rectangle: pos = CalculateRectangle(config); break;
        case EmitterShapeType::Circle:    pos = CalculateCircle(config); break;
        case EmitterShapeType::Triangle:  pos = CalculateTriangle(config); break;
        case EmitterShapeType::Cube:      pos = CalculateCube(config); break;
        case EmitterShapeType::Sphere:    pos = CalculateSphere(config); break;
        }

        // 必要に応じて、ここでエミッターの「位置」や「回転」を適用（行列計算）
        results.push_back(ApplyTransform(pos, config));
    }

    return results;
}

// 各形状の計算ロジック（前述のアルゴリズムをここに実装）
Vector3 EmitterSystem::CalculateRectangle(const EmitterConfig& config) {
    float offsetX = RandomUtils::GetInstance()->GetHighRandom().GetFloat(-config.size.x * 0.5f, config.size.x * 0.5f);
    float offsetY = RandomUtils::GetInstance()->GetHighRandom().GetFloat(-config.size.y * 0.5f, config.size.y * 0.5f);
    return { offsetX,offsetY,0.0f };
}

// 2D円の内部に均等に配置
Vector3 EmitterSystem::CalculateCircle(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    // 面積の偏りをなくすため、乱数の平方根をとる (config.size.x を半径として扱う)
    float r = config.size.x * std::sqrt(rand.GetFloat(0.0f, 1.0f));
    float theta = rand.GetFloat(0.0f, 3.14159265359f);

    return { r * std::cos(theta), r * std::sin(theta), 0.0f };
}

// 三角形の内部に均等に配置（重心座標系）
Vector3 EmitterSystem::CalculateTriangle(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    float u = rand.GetFloat(0.0f, 1.0f);
    float v = rand.GetFloat(0.0f, 1.0f);

    // 三角形の外側にはみ出た場合は、折り返すことで均等にする
    if (u + v > 1.0f) {
        u = 1.0f - u;
        v = 1.0f - v;
    }

    // ベクトルの合成
    Vector3 p;
    p.x = config.vA.x + u * (config.vB.x - config.vA.x) + v * (config.vC.x - config.vA.x);
    p.y = config.vA.y + u * (config.vB.y - config.vA.y) + v * (config.vC.y - config.vA.y);
    p.z = config.vA.z + u * (config.vB.z - config.vA.z) + v * (config.vC.z - config.vA.z);

    return p;
}

// 3D立方体の内部に均等に配置
Vector3 EmitterSystem::CalculateCube(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    float offsetX = rand.GetFloat(-config.size.x * 0.5f, config.size.x * 0.5f);
    float offsetY = rand.GetFloat(-config.size.y * 0.5f, config.size.y * 0.5f);
    float offsetZ = rand.GetFloat(-config.size.z * 0.5f, config.size.z * 0.5f);

    return { offsetX, offsetY, offsetZ };
}

// 3D球の内部に均等に配置
Vector3 EmitterSystem::CalculateSphere(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    // ガウス分布（正規分布）を使って、完全にランダムな3D方向ベクトルを作る
    double rx = rand.GetGauss(0.0, 1.0);
    double ry = rand.GetGauss(0.0, 1.0);
    double rz = rand.GetGauss(0.0, 1.0);

    float len = std::sqrt(static_cast<float>(rx * rx + ry * ry + rz * rz));

    // 万が一ゼロ除算になるのを防ぐ安全処理
    if (len < 0.0001f) {
        return { 0.0f, 0.0f, 0.0f };
    }

    // 方向の単位ベクトル化
    float dirX = static_cast<float>(rx) / len;
    float dirY = static_cast<float>(ry) / len;
    float dirZ = static_cast<float>(rz) / len;

    // 体積の偏りをなくすため、3乗根(cbrt)を使って中心からの距離を決める (config.size.x を半径として扱う)
    float d = config.size.x * std::cbrt(rand.GetFloat(0.0f, 1.0f));

    return { dirX * d, dirY * d, dirZ * d };
}


// 他の形状も同様に実装...

Vector3 EmitterSystem::ApplyTransform(const Vector3& localPos, const EmitterConfig& config) {
    Matrix4x4 worldMat = Matrix4x4::Make::Affine(
        { 1.0f, 1.0f, 1.0f }, // スケールは等倍
        config.rotation,      // エミッターの回転
        config.position       // エミッターの配置位置
    );

    // ローカル座標をワールド座標に変換して返す
    return Matrix4x4::Transform(localPos, worldMat);
}