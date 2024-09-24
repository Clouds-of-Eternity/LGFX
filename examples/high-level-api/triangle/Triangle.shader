#vertex
#version 450

//these are vertex inputs
//guarantee that each vertex of the input triangle(s) will have a position and a color element
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(position, 1.0);

    fragColor = color;
}

#fragment
#version 450

layout(location = 0) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}