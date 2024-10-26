#vertex
#version 450

layout (binding = 0) uniform ShaderGlobalData {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} globalData;

//just to match the input from the compute shader
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 velocity;

void main() 
{
    vec2 vertices[4] = vec2[4](
        vec2(-0.0625, -0.0625),
        vec2(0.0625, -0.0625),
        vec2(0.0625, 0.0625),
        vec2(-0.0625, 0.0625)
    );
    vec4 thisVertex = position + vec4(vertices[gl_VertexIndex], 0.0, 1.0);

    gl_Position = globalData.projectionMatrix * globalData.viewMatrix * thisVertex;
}

#fragment
#version 450

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(0.5, 0.5, 0.5, 1.0);
}