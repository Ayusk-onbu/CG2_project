#pragma once
#include <vector>
#include <string>
#include "../../../UsefulTool/Component/Status/Status.h"

// --- 誰を対象にするか ---
enum class TargetType {
    Enemy,      // 敵（ホーミング弾やターゲット指定）
    Self,       // 自分（バフや回復など）
    Area        // 指定座標・周囲（設置魔法など）
};

// --- どうやって発動するか（挙動） ---
enum class BehaviorType {
    Projectile, // 弾を飛ばす（スプライン軌道を使う）
    Instant,    // 即時発動（自分へのバフや、即着弾の雷など）
    Aura        // 自分の周囲に展開して追従する
};

// --- 弾1発が知るべき情報（ProjectileData） ---
struct ProjectileData {
    // --- 効果（Effect）データ ---
    float power;            // 基本威力（回復量にも流用）
    std::vector<StatusEffect> statusEffects; // バフ・デバフ効果（複数設定可）

    Element element;        // 属性
    TargetType target;
    
    std::string splineName;
    bool isHoming;
    bool isPiercing; // 貫通するかどうか（ヒットリストに関係）
};

// ==========================================
// 究極の MagicData
// ==========================================
struct MagicData {
    int magicId;            // 魔法の識別ID
    float castTime;         // 詠唱時間
    float cooldownTime;     // クールタイム
    BehaviorType behavior;

    // --- 弾（Projectile）専用データ ---
    int projectileCount;
    float spawnInterval;
    std::vector<std::string> splineJsonPaths; // 複数の軌道
    float splineDuration;

    // 弾を生み出す時は、このデータを渡す！
    ProjectileData projData;
};

