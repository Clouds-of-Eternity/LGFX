struct Particle
{
    float4 position : position;
    float4 velocity : velocity;
};
struct Matrices
{
    float4x4 viewProjection;
};

struct VertexOutput 
{
    float4 vertexPosition : SV_Position;
    float4 fragColor : fragColor;
};

ConstantBuffer<Matrices> ViewProjection;

static const float scale = 0.1f;

[shader("vertex")]
VertexOutput VertexFunction(Particle particle, uint vertexIndex: SV_VertexID)
{
    float2[] positions = {
        float2(-0.5f, -0.5f),
        float2(0.5f, -0.5f),
        float2(0.5f, 0.5f),
        float2(-0.5f, 0.5f)
    };

    float4 position = particle.position;
    position.w = 1.0f;
    position.xy += positions[vertexIndex] * scale;

    VertexOutput result;
    result.fragColor = float4(float3(particle.velocity.w), 1.0f);
    result.vertexPosition = mul(ViewProjection.viewProjection, position);
    return result;
}

[shader("fragment")]
float4 FragmentFunction(float4 color: fragColor): SV_Target
{
    return color;
}