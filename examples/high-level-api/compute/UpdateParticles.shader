#compute
#version 450

layout (binding = 0) uniform TimeData {
    float deltaTime;
} timeData;

struct Particle {
    vec4 position;
    vec4 velocity;
};

layout(std140, binding = 1) readonly buffer ParticlesIn {
   Particle particlesIn[ ];
};

layout(std140, binding = 2) buffer ParticlesOut {
   Particle particlesOut[ ];
};

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
void main() 
{
    uint index = gl_GlobalInvocationID.x;  

    Particle particleIn = particlesIn[index];

    particlesOut[index].position.xyz = particleIn.position.xyz + particleIn.velocity.xyz * timeData.deltaTime;
    particlesOut[index].velocity = particleIn.velocity;
}