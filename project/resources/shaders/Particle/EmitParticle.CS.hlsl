#include "Particle.hlsli"
#include "../Common.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
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
            int32_t freeListIndex;
            // FreeListのIndexを1つ前に設定し、現在のIndexを取得する
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < 1024)
            {
                uint32_t particleIndex = gFreeList[freeListIndex];
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
                gParticles[particleIndex].lifeTime = 5.0f;
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].velocity = (generator.Generate3d() * 2.0f - 1.0f) * 0.01f;
            }
            else
            {
                // 発生できなかったのでInterlockedAddした分を戻す
                InterlockedAdd(gFreeListIndex[0], 1);

                break;
            }
        }
    }
}