#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform texture2D tex;
layout(set = 0, binding = 2) uniform sampler texSampler;

void main() {
    outColor = texture(sampler2D(tex, texSampler), fragUV);
}