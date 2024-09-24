#vertex
#version 450

layout (binding = 0) uniform Matrices {
    mat4 projectionMatrix;
    mat4 viewMatrix;
} matrices;

//just to match the input from the compute shader
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 velocity;

layout(location = 0) out vec4 fragColor;

void main() 
{
    vec2 vertices[4] = vec2[4](
        vec2(-0.0625, -0.0625),
        vec2(0.0625, -0.0625),
        vec2(0.0625, 0.0625),
        vec2(-0.0625, 0.0625)
    );
    vec4 thisVertex = position + vec4(vertices[gl_VertexIndex], 0.0, 1.0);

    gl_Position = matrices.projectionMatrix * matrices.viewMatrix * thisVertex;

    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

#fragment
#version 450

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}