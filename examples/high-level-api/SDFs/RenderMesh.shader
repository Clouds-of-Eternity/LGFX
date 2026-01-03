#vertex
#version 450

/*
Astral.Shaderc.exe "examples\high-level-api\SDFs\RenderMesh.shader" "examples\high-level-api\SDFs\bin\Debug\RenderMesh.shaderobj"
*/

layout(binding = 0) uniform ShaderGlobalData
{
    mat4 projection;
    mat4 view;
    mat4 projectionInverse;
    mat4 viewInverse;
    vec4 minExtents;
    vec4 maxExtents;
} globalData;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

layout(location = 0) out vec3 fragNormal;

void main() 
{
    gl_Position = globalData.projection * globalData.view * vec4(position.xyz, 1.0);
    fragNormal = normal.xyz;
}

#fragment
#version 450
#line 31

layout(location = 0) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4((fragNormal + vec3(1.0)) * 0.5, 1.0);
}