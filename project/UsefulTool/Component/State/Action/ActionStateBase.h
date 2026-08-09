#pragma once
#include "InputHandler.h"
#include <string>

class DynamicObject;

class ActionStateBase {
public:
    virtual ~ActionStateBase() = default;

    virtual void OnEnter(DynamicObject* owner) {}
    virtual void OnUpdate(DynamicObject* owner, const CommandState& input, float deltaTime) = 0;
    virtual void OnExit(DynamicObject* owner) {}

    // --- 移動システムへの介入設定 ---
    // このアクション中に移動を許可するか？（大技や硬直中は false、歩き攻撃なら true）
    virtual bool IsMovementAllowed() const { return true; }

    // 移動許可時の速度倍率（例: ガードや構え中は 0.5 倍速）
    virtual float GetSpeedMultiplier() const { return 1.0f; }

    // デバッグ表示用
    virtual const std::string& GetName() const = 0;
};

//class MeleeAttackActionState : public ActionStateBase {
//public:
//    MeleeAttackActionState(float motionMult = 1.2f, float duration = 0.8f)
//        : motionMultiplier_(motionMult), totalDuration_(duration) {
//    }
//
//    void OnEnter(DynamicObject* owner) override {
//        timer_ = 0.0f;
//
//        // 1. 攻撃判定（ColliderComponent）の HitHistory をリセット！
//        if (auto* col = owner->GetComponent<ColliderComponent>()) {
//            col->ResetHitHistory();
//        }
//
//        // 2. DamageSourceComponent にこの技の「モーション倍率」を設定
//        if (auto* dmgSource = owner->GetComponent<DamageSourceComponent>()) {
//            dmgSource->SetMotionMultiplier(motionMultiplier_);
//        }
//    }
//
//    void OnUpdate(DynamicObject* owner, const CommandState& input, float deltaTime) override {
//        timer_ += deltaTime;
//
//        // モーション時間が終わったら自由状態（Default）へ抜ける処理など
//        if (timer_ >= totalDuration_) {
//            // アクション終了（ActionComponent側でFreeに戻すか、Exitアニメーションへ）
//        }
//    }
//
//    // 斬りつけ中は移動不可（固定）
//    bool IsMovementAllowed() const override { return false; }
//
//    const std::string& GetName() const override {
//        static std::string name = "MeleeAttack";
//        return name;
//    }
//
//private:
//    float motionMultiplier_ = 1.2f;
//    float totalDuration_ = 0.8f;
//    float timer_ = 0.0f;
//};

//{
//    struct AIAttackOption {
//        std::string name;
//        float baseWeight;       // 技の基本優先度（例: 大技=30, 小技=100）
//        float minRange;         // 発動可能な最小距離
//        float maxRange;         // 発動可能な最大距離
//        float cooldown;         // クールダウン時間（秒）
//        float currentCooldown;  // 残りクールダウン時間
//
//        ButtonState triggerButton; // 割り当てる入力ボタン (mainAction等)
//    };
//
//    class AIAttackSelector {
//    public:
//        AIAttackSelector() = default;
//
//        void AddOption(const AIAttackOption& opt) {
//            options_.push_back(opt);
//        }
//
//        void UpdateCooldowns(float deltaTime) {
//            for (auto& opt : options_) {
//                if (opt.currentCooldown > 0.0f) {
//                    opt.currentCooldown -= deltaTime;
//                }
//            }
//        }
//
//        // --- 最適な攻撃コマンドの選定 ---
//        ButtonState SelectBestAttack(float distanceToTarget, float aggressiveness) {
//            float maxScore = -1.0f;
//            AIAttackOption* bestOption = nullptr;
//
//            for (auto& opt : options_) {
//                // クールダウン中なら選外
//                if (opt.currentCooldown > 0.0f) continue;
//
//                // 射程外なら選外
//                if (distanceToTarget < opt.minRange || distanceToTarget > opt.maxRange) continue;
//
//                // 距離適性スコア (射程中央に近いほど高スコア)
//                float midRange = (opt.minRange + opt.maxRange) * 0.5f;
//                float rangeFactor = 1.0f - (std::abs(distanceToTarget - midRange) / (opt.maxRange + 0.1f));
//                rangeFactor = (std::max)(0.1f, rangeFactor);
//
//                // ★ 総合スコア計算： 重み × 積極性 × 距離適性
//                float score = opt.baseWeight * aggressiveness * rangeFactor;
//
//                if (score > maxScore) {
//                    maxScore = score;
//                    bestOption = &opt;
//                }
//            }
//
//            if (bestOption) {
//                // 決定した技のクールダウンを開始
//                bestOption->currentCooldown = bestOption->cooldown;
//                return bestOption->triggerButton;
//            }
//
//            return ButtonState::None;
//        }
//
//    private:
//        std::vector<AIAttackOption> options_;
//    };
//}