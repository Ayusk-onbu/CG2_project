#include "EmitterSystem.h"
#include "RandomUtils.h"
#include "TextureManager.h"

// ---------------------------------------------------------
// 全体生成処理
// ---------------------------------------------------------
std::vector<EmissionResult> EmitterSystem::GeneratePositions(const EmitterConfig& config, uint32_t count) {
    std::vector<EmissionResult> results;
    results.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        EmissionResult localRes = {};

        switch (config.shapeType) {
        case EmitterShapeType::Rectangle: localRes = CalculateRectangle(config); break;
        case EmitterShapeType::Circle:    localRes = CalculateCircle(config); break;
        case EmitterShapeType::Triangle:  localRes = CalculateTriangle(config); break;
        case EmitterShapeType::Cube:      localRes = CalculateCube(config); break;
        case EmitterShapeType::Sphere:    localRes = CalculateSphere(config); break;
        case EmitterShapeType::Noise: localRes = CalculateNoise(config); break;
        }

        // ワールド空間の座標と速度に変換して格納
        results.push_back(ApplyTransform(localRes, config));
    }

    return results;
}

// ---------------------------------------------------------
// 各形状の計算ロジック（位置と、中心から外へ向かう初期速度を算出）
// ---------------------------------------------------------

// 四角形 (Rectangle)
EmissionResult EmitterSystem::CalculateRectangle(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();
    Vector3 p = { 0.0f, 0.0f, 0.0f };

    if (config.space == EmitSpace::Volume) {
        // 内部全体
        p.x = rand.GetFloat(-config.size.x * 0.5f, config.size.x * 0.5f);
        p.y = rand.GetFloat(-config.size.y * 0.5f, config.size.y * 0.5f);
    }
    else {
        // 表面（4つの辺のどこか）
        int edge = rand.GetInt(0, 3); // 0:右, 1:左, 2:上, 3:下
        float t = rand.GetFloat(-0.5f, 0.5f);

        switch (edge) {
        case 0: p = { 0.5f, t, 0.0f }; break;
        case 1: p = { -0.5f, t, 0.0f }; break;
        case 2: p = { t,  0.5f, 0.0f }; break;
        case 3: p = { t, -0.5f, 0.0f }; break;
        }
        p.x *= config.size.x;
        p.y *= config.size.y;
    }

    // 速度（中心から外側へのベクトル。中心にいる場合は上に飛ばす）
    float len = std::sqrt(p.x * p.x + p.y * p.y);
    Vector3 dir = (len > 0.0001f) ? Vector3{ p.x / len, p.y / len, 0.0f } : Vector3{ 0.0f, 1.0f, 0.0f };

    return { p, dir * config.initialSpeed };
}

// 円 (Circle)
EmissionResult EmitterSystem::CalculateCircle(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    float r = config.size.x;
    if (config.space == EmitSpace::Volume) {
        r *= std::sqrt(rand.GetFloat(0.0f, 1.0f)); // 偏り防止
    }

    float theta = rand.GetFloat(0.0f, 3.14159265359f * 2.0f); // 360度

    Vector3 p = { r * std::cos(theta), r * std::sin(theta), 0.0f };

    Vector3 dir = { std::cos(theta), std::sin(theta), 0.0f };
    return { p, dir * config.initialSpeed };
}

// 三角形 (Triangle)
EmissionResult EmitterSystem::CalculateTriangle(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();
    Vector3 p;

    if (config.space == EmitSpace::Volume) {
        // 内部全体（重心座標系）
        float u = rand.GetFloat(0.0f, 1.0f);
        float v = rand.GetFloat(0.0f, 1.0f);
        if (u + v > 1.0f) { u = 1.0f - u; v = 1.0f - v; }

        p.x = config.vA.x + u * (config.vB.x - config.vA.x) + v * (config.vC.x - config.vA.x);
        p.y = config.vA.y + u * (config.vB.y - config.vA.y) + v * (config.vC.y - config.vA.y);
        p.z = config.vA.z + u * (config.vB.z - config.vA.z) + v * (config.vC.z - config.vA.z);
    }
    else {
        // 表面（3つの辺のどこか）
        int edge = rand.GetInt(0, 2);
        float t = rand.GetFloat(0.0f, 1.0f);
        Vector3 start, end;

        if (edge == 0) { start = config.vA; end = config.vB; }
        else if (edge == 1) { start = config.vB; end = config.vC; }
        else { start = config.vC; end = config.vA; }

        p.x = start.x + (end.x - start.x) * t;
        p.y = start.y + (end.y - start.y) * t;
        p.z = start.z + (end.z - start.z) * t;
    }

    // 速度（三角形の中心（重心）から外側へのベクトル）
    Vector3 center = {
        (config.vA.x + config.vB.x + config.vC.x) / 3.0f,
        (config.vA.y + config.vB.y + config.vC.y) / 3.0f,
        (config.vA.z + config.vB.z + config.vC.z) / 3.0f
    };
    Vector3 diff = { p.x - center.x, p.y - center.y, p.z - center.z };
    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    Vector3 dir = (len > 0.0001f) ? Vector3{ diff.x / len, diff.y / len, diff.z / len } : Vector3{ 0.0f, 1.0f, 0.0f };

    return { p, dir * config.initialSpeed };
}

// 立方体 (Cube)
EmissionResult EmitterSystem::CalculateCube(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();
    Vector3 p = { 0.0f, 0.0f, 0.0f };

    if (config.space == EmitSpace::Volume) {
        // 内部全体
        p.x = rand.GetFloat(-config.size.x * 0.5f, config.size.x * 0.5f);
        p.y = rand.GetFloat(-config.size.y * 0.5f, config.size.y * 0.5f);
        p.z = rand.GetFloat(-config.size.z * 0.5f, config.size.z * 0.5f);
    }
    else {
        // 表面（6つの面のどこか）
        int face = rand.GetInt(0, 5);
        float t1 = rand.GetFloat(-0.5f, 0.5f);
        float t2 = rand.GetFloat(-0.5f, 0.5f);

        switch (face) {
        case 0: p = { 0.5f, t1, t2 }; break; // +X面
        case 1: p = { -0.5f, t1, t2 }; break; // -X面
        case 2: p = { t1,  0.5f, t2 }; break; // +Y面
        case 3: p = { t1, -0.5f, t2 }; break; // -Y面
        case 4: p = { t1, t2,  0.5f }; break; // +Z面
        case 5: p = { t1, t2, -0.5f }; break; // -Z面
        }
        p.x *= config.size.x;
        p.y *= config.size.y;
        p.z *= config.size.z;
    }

    float len = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    Vector3 dir = (len > 0.0001f) ? Vector3{ p.x / len, p.y / len, p.z / len } : Vector3{ 0.0f, 1.0f, 0.0f };

    return { p, dir * config.initialSpeed };
}

// 球体 (Sphere)
EmissionResult EmitterSystem::CalculateSphere(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    double rx = rand.GetGauss(0.0, 1.0);
    double ry = rand.GetGauss(0.0, 1.0);
    double rz = rand.GetGauss(0.0, 1.0);

    float len = std::sqrt(static_cast<float>(rx * rx + ry * ry + rz * rz));
    if (len < 0.0001f) return { {0.0f,0.0f,0.0f}, {0.0f,config.initialSpeed,0.0f} };

    Vector3 dir = { static_cast<float>(rx) / len, static_cast<float>(ry) / len, static_cast<float>(rz) / len };

    float d = config.size.x;
    if (config.space == EmitSpace::Volume) {
        d *= std::cbrt(rand.GetFloat(0.0f, 1.0f)); // 3乗根で体積の偏り防止
    }

    return { { dir.x * d, dir.y * d, dir.z * d }, dir * config.initialSpeed };
}

// Noise
EmissionResult EmitterSystem::CalculateNoise(const EmitterConfig& config) {
    auto& rand = RandomUtils::GetInstance()->GetHighRandom();

    // 安全装置：テクスチャ名が空なら、とりあえずRectangleとして処理して返す
    if (config.noiseTextureName.empty()) {
        return CalculateRectangle(config);
    }

    // マネージャーからノイズテクスチャを取得
    Texture& noiseTex = TextureManager::GetInstance()->GetTexture(config.noiseTextureName);

    Vector3 p = { 0.0f, 0.0f, 0.0f };
    float finalNoiseVal = 0.0f;
    bool isSuccess = false;

    // 【拒絶サンプリング】
    // 条件（閾値）に合うピクセルが見つかるまで最大100回ランダムに引き直す
    // ※無限ループでフリーズするのを防ぐために上限を設けます
    for (int retry = 0; retry < 100; ++retry) {
        // 0.0 ～ 1.0 のUV座標をランダムに決定
        float u = rand.GetFloat(0.0f, 1.0f);
        float v = rand.GetFloat(0.0f, 1.0f);

        // 先ほど作成した関数で、CPU側からテクスチャの明るさをサンプリング
        float noiseVal = noiseTex.SampleNoiseCPU(u, v);

        // 設定された閾値より明るい（＝ノイズの模様が出ている）場所なら合格！
        if (noiseVal >= config.noiseThreshold) {
            // UV(0～1) から、エミッターのサイズ(size.x, size.y)に応じたローカル座標に変換
            p.x = (u - 0.5f) * config.size.x;
            p.y = (v - 0.5f) * config.size.y;
            p.z = 0.0f; // 2DテクスチャベースなのでいったんZは0

            finalNoiseVal = noiseVal;
            isSuccess = true;
            break;
        }
    }

    // もし100回探しても見つからなかった場合は、諦めて範囲内の適当な場所に落とす
    if (!isSuccess) {
        float u = rand.GetFloat(0.0f, 1.0f);
        float v = rand.GetFloat(0.0f, 1.0f);
        p.x = (u - 0.5f) * config.size.x;
        p.y = (v - 0.5f) * config.size.y;
        finalNoiseVal = config.noiseThreshold;
    }

    // 速度の設定：
    // ノイズの形（例えば稲妻や炎のうねり）に沿うように、
    // 「ノイズが濃い（白い）場所ほど勢いよく飛び出す」という演出にするため、初速にノイズ値を掛け算します
    Vector3 dir = { 0.0f, 1.0f, 0.0f }; // デフォルトは上向き（必要に応じて変更可能）

    return { p, dir * (config.initialSpeed * finalNoiseVal) };
}

// ---------------------------------------------------------
// 変換処理
// ---------------------------------------------------------
EmissionResult EmitterSystem::ApplyTransform(const EmissionResult& localRes, const EmitterConfig& config) {
    // 座標用：スケール・回転・平行移動を含むアフィン変換行列
    Matrix4x4 worldMat = Matrix4x4::Make::Affine({ 1.0f, 1.0f, 1.0f }, config.rotation, config.position);

    // 速度用：回転のみの行列（速度ベクトルに平行移動は適用しないため）
    Matrix4x4 rotMat = Matrix4x4::Make::RotateXYZ(config.rotation);

    EmissionResult result;
    // 座標はワールド行列で変換
    result.position = Matrix4x4::Transform(localRes.position, worldMat);

    // 速度ベクトルは「回転」だけを適用する
    result.velocity = Matrix4x4::Transform(localRes.velocity, rotMat);

    return result;
}