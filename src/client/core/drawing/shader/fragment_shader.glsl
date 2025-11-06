#version 450

layout(set = 3, binding = 0) uniform FragmentPushConstants {
    vec3 lightDir;
    float useTexture;
} pc;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

void main() {
    vec3 lightDirection = pc.lightDir.xyz;

    vec4 baseColor = fragColor;
    vec4 texColor = texture(texSampler, fragUV);

    // Use mix to avoid branching issues
    baseColor = mix(baseColor, baseColor * texColor, pc.useTexture);

    float diff = max(dot(fragNormal, -lightDirection), 0.0);
    vec3 ambient = vec3(0.5);
    vec3 lighting = ambient + diff * vec3(0.5);

    outColor = vec4(baseColor.rgb * lighting, baseColor.a);
}