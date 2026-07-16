#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);

[numthreads(1024, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint32_t particleIndex = dispatchThreadID.x;
    if (particleIndex < 1024)
    {
        //Particle構造体の全要素を0で埋めるという書き方
        gParticles[particleIndex] = (Particle)0;
        gParticles[particleIndex].scale = float32_t3(0.5f, 0.5f, 0.5f);
        //gParticles[particleIndex].color = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);
        
        if (particleIndex == 0)
        {
            gFreeCounter[0] = 0;
        }

    }
}