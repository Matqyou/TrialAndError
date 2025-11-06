#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;
layout(set = 3, binding = 0) uniform PushConstants {
    float useTexture;
} pc;

void main() {
    vec4 baseColor = fragColor;

    if (pc.useTexture > 0.5) {
        vec4 texColor = texture(texSampler, fragUV);
        baseColor *= texColor;
    }

    outColor = baseColor;
}
