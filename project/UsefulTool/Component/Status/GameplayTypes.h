#pragma once

// --- 属性の定義 ---
enum class Element {
    None,       // 無属性
    Fire,       // 炎
    Water,      // 水
    Ice,        // 氷
    Wind,       // 風
    Lightning,  // 雷
    Earth,      // 土
    Light,      // 光
    Dark        // 闇
};

enum class StatusType {
    Attack,          // 攻撃力
    Defense,         // 防御力
    Speed,           // 移動速度
    MaxHealth,       // 最大HP（バフ用）
    SlipDamage,      // 毒や燃焼などのスリップダメージ
    Stun             // スタン（麻痺・行動不能）
};

// --- バフ・デバフの詳細データ ---
struct StatusEffect {
    StatusType statusType;   // 何を変化させるか
    float multiplier; // 倍率（例: 1.2f なら20%アップ、0.8f なら20%ダウン）
    float duration;   // 効果時間（秒）
};