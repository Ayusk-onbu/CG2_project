#include "ParticleEmitter.h"
#include "Particle.h"
#include "CameraSystem.h"

void ParticleEmitter::Initialize(Fngine* fngine) {
    fngine_ = fngine;
    obj_ = std::make_unique<ModelObject>();
    obj_->modelName_ = "bullet";
    obj_->textureName_ = "bullet";
    obj_->Initialize(fngine);
}

void ParticleEmitter::Update(Particle* particleSystem) {
	for (uint32_t i = 0;i < emitNum_;++i) {
		Emit(particleSystem);
	}
    Vector3 pos = worldTransform_.get_.Translation();
    int num = emitNum_;
    std::string name = "ParticleEmiiter" + name_;
    ImGui::Begin(name.c_str());
    ImGui::DragFloat3("pos", &pos.x);
    ImGui::DragFloat("spawnRadius", &spawnRadius_);
    ImGui::DragFloat("minLife", &minLifeTime_,1.0f,0.0f);
    ImGui::DragFloat("maxLife", &maxLifeTime_);
    ImGui::DragFloat3("minSpeed", &minVelocity_.x);
    ImGui::DragFloat3("maxSpeed", &maxVelocity_.x);
    ImGui::SliderInt("emitNum", &num, 0, 100);
    ImGui::ColorEdit4("Start color", &startColor_.x);
    ImGui::ColorEdit4("End color", &endColor_.x);
    ImGui::End();
    worldTransform_.set_.Translation(pos);
    emitNum_ = num;
}

void ParticleEmitter::Emit(Particle* particleSystem) {
    // particleSystem (Particleクラス)のinfo_リストにアクセスして生成
    if (particleSystem->GetInfoList().size() < particleSystem->GetNumMaxInstance()) {
        std::unique_ptr<ParticleData> info = std::make_unique<ParticleData>();

        // ------------------------------------
        // 1. 位置とスケール
        // ------------------------------------
        info->worldTransform.Initialize();

        // エミッターの位置を基準に、spawnRadius_分ランダムにずらした位置を初期位置とする
        Vector3 spawnOffset = {
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(-spawnRadius_, spawnRadius_),
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(-spawnRadius_, spawnRadius_),
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(-spawnRadius_, spawnRadius_)
        };

        info->worldTransform.set_.Translation(worldTransform_.get_.Translation() + spawnOffset);

        info->startScale = RandomUtils::GetInstance()->GetHighRandom().GetFloat(3.0f, 4.5f);
        info->endScale = RandomUtils::GetInstance()->GetHighRandom().GetFloat(0.3f, 0.45f);

        // ------------------------------------
        // 2. 速度
        // ------------------------------------
        info->velocity = {
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(minVelocity_.x, maxVelocity_.x),
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(minVelocity_.y, maxVelocity_.y),
            RandomUtils::GetInstance()->GetHighRandom().GetFloat(minVelocity_.z, maxVelocity_.z)
        };

        // ------------------------------------
        // 3. 色と寿命
        // ------------------------------------
        info->startColor = startColor_;

        info->endColor = endColor_;

        info->lifeTime = RandomUtils::GetInstance()->GetHighRandom().GetFloat(minLifeTime_, maxLifeTime_);
        info->currentTime = 0.0f;

        // WorldTransformの初期化を完了
        info->worldTransform.LocalToWorld();

        // ParticleSystemのリストに追加
        particleSystem->AddParticle(std::move(info));
    }
}

void ParticleEmitter::DrawDebug() {
    obj_->worldTransform_.set_.Translation(worldTransform_.get_.Translation());
    obj_->worldTransform_.set_.Scale({spawnRadius_,spawnRadius_,spawnRadius_});
    obj_->LocalToWorld();
    obj_->SetWVPData(CameraSystem::GetInstance()->GetActiveCamera()->DrawCamera(obj_->worldTransform_.mat_));
    obj_->Draw(ObjectDrawType::WIREFRAME);
}