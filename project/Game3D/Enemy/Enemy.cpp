#include "Enemy.h"
#include "CameraSystem.h"

void BossEnemy::Initialize(Fngine* fngine, Player3D* target) {

    target_ = target;

    obj_ = std::make_unique<ModelObject>();
    obj_->Initialize(fngine->GetD3D12System(), "cube.obj");
    obj_->SetFngine(fngine);
    obj_->textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
    obj_->worldTransform_.set_.Translation({ 0.0f,0.0f,30.0f });

    // HPを初期化
    hp_ = maxHp_;

    // Colliderの初期化
    bodyCollider_ = std::make_unique<BossBodyCollider>(this);
    bodyCollider_->SetRadius(bossBodyRadius);

    bodyColliderObj_ = std::make_unique<ModelObject>();
    bodyColliderObj_->Initialize(fngine->GetD3D12System(), "cube.obj");
    bodyColliderObj_->SetFngine(fngine);
    bodyColliderObj_->worldTransform_.set_.Scale({ bossBodyRadius,bossBodyRadius,bossBodyRadius });
    bodyColliderObj_->SetColor({ 0.5f,0.0f,0.0f,1.0f });
    bodyColliderObj_->textureHandle_ = TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");

    attackCollider_ = std::make_unique<AttackCollider>();
    attackCollider_->SetMyType(COL_None);
    attackCollider_->SetYourType(COL_None);
    attackCollider_->SetWorldPosition({ 0.0f,0.0f,30.0f });

    isViewAttack_ = false;

    bullets_ = std::make_unique<BulletManager>();
    bullets_->SetFngine(fngine);

    ChangeState(new BossDecisionState);
}

void BossEnemy::Update(){
    // 1. フレーム開始時、前フレームの移動量をリセット（新しい移動量が設定されない場合に備えて）
    moveAmount_ = { 0.0f, 0.0f, 0.0f };

    // 2. Stateの更新 (ここで State が SetMovement を呼び出す)
    if (state_) {
        state_->Update();
    }

    // 3. 実際の移動の適用
    obj_->worldTransform_.set_.Translation(obj_->worldTransform_.get_.Translation() + moveAmount_);

    bullets_->Update();

    ImGuiManager::GetInstance()->DrawDrag("Boss : HP",hp_);
    ImGuiManager::GetInstance()->DrawDrag("Boss : mat", obj_->worldTransform_.mat_);
}

void BossEnemy::Draw(){
    obj_->LocalToWorld();
    obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
    obj_->Draw();

    if (isViewAttack_) {
        bodyColliderObj_->worldTransform_.set_.Translation(attackCollider_->GetWorldPosition());
        bodyColliderObj_->LocalToWorld();
        bodyColliderObj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(bodyColliderObj_->worldTransform_.mat_));
        bodyColliderObj_->Draw();
    }

    bullets_->Draw();
}

void BossEnemy::TakeDamage(float damage) {
    // 死んでいたら無視
    if (IsDead()) { return; }

    // HPを減らす
    hp_ -= damage;

    // HPの下限をチェック
    if (hp_ < 0.0f) {
        hp_ = 0.0f;
    }

    // 状態遷移の判断
    if (IsDead()) {
        // HPが０になったら死亡状態へ
        ChangeState(new BossDeathState());
        return;
    }
    else {
        // のけぞり状態にしたっていい
        ChangeState(new BossHurtState());
        return;
    }
}

void BossEnemy::EnableAttackHitBox(bool enable, const Vector3& worldPos, float radius)
{
    if (!attackCollider_) return;

    if (enable) {
        // 1. 位置と半径を設定
        attackCollider_->SetWorldPosition(worldPos);
        attackCollider_->SetRadius(radius);

        // 2. 攻撃判定をアクティブにする
        // 自身の属性をボスの攻撃 (COL_ENEMY_ATTACK) に設定
        attackCollider_->SetMyType(COL_Enemy_Attack);

        // 3. マスクを設定
        // プレイヤー本体 (COL_PLAYER) に当たるように設定
        attackCollider_->SetYourType(COL_Player);
    }
    else {
        // 攻撃判定を非アクティブにする
        // 自身の属性を COL_NONE に設定することで、CollisionManagerでのチェックをスキップさせる
        attackCollider_->SetMyType(COL_None);
        attackCollider_->SetYourType(COL_None);
    }
}

void BossEnemy::ChangeState(BossState* newState) {
    if (state_) {
        state_->Exit();

        delete state_;
        state_ = nullptr;
    }
    state_ = newState;
    state_->SetBoss(this);
    state_->Initialize();
}

void BossEnemy::LookAtTarget()
{
    // 1. 方向ベクトルを計算 (水平方向のみ)
    Vector3 bossPos = obj_->worldTransform_.GetWorldPos();
    Vector3 targetPos = GetTargetPosition();

    if (std::isnan(targetPos.x) || std::isnan(targetPos.y) || std::isnan(targetPos.z)) {
        // プレイヤーがNaNなので、これ以上計算せず、回転処理をスキップ
        return;
    }

    Vector3 direction = targetPos - bossPos;
    direction.y = 0.0f; // 水平方向のみを考慮
    direction = Normalize(direction); // Normalize関数を使用

    // 2. Y軸周りの回転角度を計算 (Z軸を前方基準として)
    // atan2(X, Z) はZ軸正方向からY軸周りの回転角度を求める
    float angle = std::atan2(direction.x, direction.z);

    // 3. Y軸回転のクォータニオンを生成
    Quaternion targetRotationQ = Quaternion::MakeRotateAxisAngleQuaternion({ 0.0f, 1.0f, 0.0f }, angle);

    // 4. WorldTransformに設定
    obj_->worldTransform_.set_.Quaternion(targetRotationQ);
}

Vector3 BossEnemy::GetForwardVector()
{
    // 1. 現在の回転クォータニオンを取得
    Quaternion currentRotationQ = obj_->worldTransform_.get_.Quaternion();

    // 2. ローカル座標系での前方ベクトル（Z軸）
    Vector3 forward = { 0.0f, 0.0f, 1.0f };

    // 3. クォータニオンを使ってベクトルを回転させる
    Vector3 worldForward = Quaternion::RotateVector(forward, currentRotationQ);

    // 4. 必要に応じて正規化（移動に使う場合、既に正規化されている可能性が高いが念のため）
    // worldForward = Normalize(worldForward); 

    return worldForward;
}