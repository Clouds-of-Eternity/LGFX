struct Particle
{
    float4 position;
    float4 velocity;
};
struct ContextData
{
    uint particlesCount;
    float deltaTime;
};

StructuredBuffer<Particle> ParticlesIn;
RWStructuredBuffer<Particle> ParticlesOut;
ConstantBuffer<ContextData> Context;

[shader("compute")]
[numthreads(256, 1, 1)]
void main(uint3 threadID: SV_DispatchThreadID)
{
    if (threadID.x >= Context.particlesCount)
    {
        return;
    }
    Particle particle = ParticlesIn[threadID.x];
    particle.position.xyz += particle.velocity.xyz * Context.deltaTime;

    ParticlesOut[threadID.x] = particle;
}