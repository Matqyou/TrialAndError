#version 450

layout(set = 1, binding = 0) uniform UniformBuffer {
    mat4 model;
    mat4 view;
    mat4 projection;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec3 fragNormal;

void main() {
    gl_Position = pc.projection * pc.view * pc.model * vec4(inPosition, 1.0);
    fragUV = inUV;
    fragColor = inColor;

    // Transform normal to world space
    fragNormal = normalize(mat3(pc.model) * aNormal);
}