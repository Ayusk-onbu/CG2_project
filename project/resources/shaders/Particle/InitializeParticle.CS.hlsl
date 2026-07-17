#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint32_t particleIndex = dispatchThreadID.x;
    if (particleIndex < 1024)
    {
        //Particle構造体の全要素を0で埋めるという書き方
        gParticles[particleIndex] = (Particle)0;
        gParticles[particleIndex].scale = float32_t3(0.5f, 0.5f, 0.5f);
        gParticles[particleIndex].color = float32_t4(1.0f, 1.0f, 1.0f, 0.0f);
        gParticles[particleIndex].lifeTime = 5.0f;
        gParticles[particleIndex].velocity = float32_t3(0.01f, 0.01f, 0.0f);
        gParticles[particleIndex].currentTime = 0.0f;
        
        gFreeList[particleIndex] = particleIndex;
        if (particleIndex == 0)
        {
            // MaxInstance - 1
            gFreeListIndex[0] = 1024 -1;
        }

    }
}