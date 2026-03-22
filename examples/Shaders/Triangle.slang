struct VertexInput
{
    float4 position;
    float4 color;
}

struct VertexOutput 
{
    float4 vertexPosition : SV_Position;
    float4 fragColor : fragColor;
};

[shader("vertex")]
VertexOutput VertexFunction(VertexInput input)
{
    VertexOutput result;
    result.fragColor = input.color;
    result.vertexPosition = float4(input.position.xyz, 1.0);
    return result;
}

[shader("fragment")]
float4 FragmentFunction(float4 color: fragColor): SV_Target
{
    return color;
}