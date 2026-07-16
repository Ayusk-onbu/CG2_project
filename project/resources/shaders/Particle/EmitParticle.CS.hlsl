#include "Particle.hlsli"
#include "../Common.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);
ConstantBuffer<Emitter> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (gEmitter.emit != 0)
    { //射出許可が出たので射出
        RandomGenerator generator;
        generator.seed = (dispatchThreadID + gPerFrame.time) * gPerFrame.time;
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int32_t particleIndex;
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            if (particleIndex < 1024)
            {
                //カウント分Particleを射出する
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
                //他のパラメータも初期化しておこう
            }
        }
    }
}