#pragma once
#include "Chronos.h"
#include "../Base/GPUComputeBaseModel.h"

struct GPUParticleForGPU {
	Vector3 translate;
	Vector3 scale;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

// エミッターの構造体データ
struct GPUEmitter {
    uint32_t count;// 射出数
    uint32_t emit; // 射出許可
};

class GPUParticleSystem : public GPUComputeBaseModel<GPUParticleForGPU>
{
public:
    void Initialize(Fngine* engine, uint32_t numParticles);

    void Update(float deltaTime);

protected:
    // 初期化用のCS（全パーティクルの寿命を0にして、起動直後に一斉リスポーンさせる）
    void DispatchInitializeCS() override;

    // 毎フレームの更新用CS
    void DispatchUpdateCS() override;

private:
    float time_ = 0.0f;
    std::unique_ptr<ConstantBuffer<PerView>>perViewBuffer_;
    std::unique_ptr<ConstantBuffer<GPUEmitter>>emitBuffer_;
    std::unique_ptr<ConstantBuffer<PerFrame>>perFrameBuffer_;
    std::unique_ptr<RWStructured<int>> freeListIndexBuffer_;
    std::unique_ptr<RWStructured<uint32_t>> freeListBuffer_;
    int timeIndex_;
};