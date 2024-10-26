#vertex
#version 450

layout(location = 0) out vec2 fragTexCoord;

void main() 
{
    vec2 vertices[4] = vec2[4](
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0),
        vec2(1.0, 1.0),
        vec2(-1.0, 1.0)
    );
    vec4 thisVertex = vec4(vertices[gl_VertexIndex], 0.0, 1.0);

    gl_Position = thisVertex;
    fragTexCoord = (vertices[gl_VertexIndex] + vec2(1.0)) * 0.5;
}

#fragment
#version 450

layout (binding = 0) uniform ShaderGlobalData {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} globalData;
layout(binding = 1) uniform texture2D noiseMap;
layout(binding = 2) uniform sampler samplerState;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

float sphere(vec3 p, float r, out vec3 normal, out vec3 color) 
{
    float noise = texture(sampler2D(noiseMap, samplerState), fragTexCoord).r;
    normal = normalize(vec3(0.0) - p);
    color = vec3(noise + 0.5);
    return length(p) - r;
}
vec3 raymarch(vec3 pos, vec3 raydir) 
{
    const vec3 lightdir = normalize(vec3(1.0, 1.0, 1.0));
    int step = 0;
    vec3 normal = vec3(0.0);
    vec3 color = vec3(0.0);
    float d = sphere(pos, 0.5, normal, color);
    while (abs(d) > 0.001 && step < 50) 
    {
        pos = pos + raydir * d;
        d = sphere(pos, 0.5, normal, color); // Return sphere(pos) or any other
        step++;
    }
    if (step < 50)
    {
        //normal of sphere = 
        return vec3(dot(lightdir, normal)) * color;
    }
    return vec3(0.0);
}

void main() 
{
    vec2 UV = (gl_FragCoord.xy - vec2(1920.0, 1080.0) * 0.5) / (1080.0 * 0.5);
    vec4 rayOrigin = globalData.viewMatrix * vec4(UV.x, UV.y, 0.0, 1.0);//globalData.viewMatrix[3].xyz;
    rayOrigin.xyz /= rayOrigin.w;
    vec4 raydir = vec4(0.0, 0.0, 1.0, 1.0);//globalData.projectionMatrix * vec4(UV.x, UV.y, 0.5, 1.0);
    outColor = vec4(raymarch(rayOrigin.xyz, raydir.xyz / raydir.w), 1.0);
}