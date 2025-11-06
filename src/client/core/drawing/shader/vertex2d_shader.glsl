#version 450

layout(location = 0) in vec2 inPosition;   // Local quad coordinates
layout(location = 1) in vec2 inUV;         // Texture coordinates
layout(location = 2) in vec4 inColor;      // Vertex color

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;

layout(set = 1, binding = 0) uniform PushConstants {
    vec2 resolution;    // Screen resolution
    vec2 translation;   // Translation (top-left corner in pixels)
    float rotation;     // Rotation in radians
} pc;

void main() {
    fragColor = inColor;
    fragUV = inUV;

    // Precompute sin/cos once per vertex
    float c = cos(pc.rotation);
    float s = sin(pc.rotation);

    // Apply rotation around origin
    vec2 rotatedPos;
    rotatedPos.x = inPosition.x * c - inPosition.y * s;
    rotatedPos.y = inPosition.x * s + inPosition.y * c;

    // Apply translation
    vec2 finalPos = rotatedPos + pc.translation;

    // Convert to NDC coordinates (0,0 = top-left)
    vec2 ndc;
    ndc.x = (finalPos.x / pc.resolution.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (finalPos.y / pc.resolution.y) * 2.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
}
