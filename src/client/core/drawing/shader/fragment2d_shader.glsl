#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;
layout(set = 3, binding = 0) uniform PushConstants {
    float useTexture;
    float useCircle;  // Add this: 0.0 = quad, 1.0 = circle
} pc;

void main() {
    vec4 baseColor = fragColor;

    // Circle mode
    if (pc.useCircle > 0.5) {
        // Map UV from (0,1) to (-1,1) with center at origin
        vec2 coord = fragUV * 2.0 - 1.0;

        // Calculate distance from center
        float dist = length(coord);

        // Discard pixels outside circle
        if (dist > 1.0) {
            discard;
        }

        // Optional: Anti-aliasing on edges
        float edgeSoftness = 0.02;
        float alpha = 1.0 - smoothstep(1.0 - edgeSoftness, 1.0, dist);
        baseColor.a *= alpha;
    }

    // Texture mode
    if (pc.useTexture > 0.5) {
        vec4 texColor = texture(texSampler, fragUV);
        baseColor *= texColor;
    }

    outColor = baseColor;
}