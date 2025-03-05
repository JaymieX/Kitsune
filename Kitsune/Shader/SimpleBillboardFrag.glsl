#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(fragment)

layout (location = 0) in vec2 fragOffset;
layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projectionMatrix;
    mat4 ViewMatrix;
    vec3 directionToLight;
    vec4 ambientColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;
} ubo;

void main() {
    float dis = sqrt(dot(fragOffset, fragOffset));
    if (dis >= 1.0) {
        discard;
    }
    outColor = vec4(ubo.pointLightColor.xyz, 1.0);
}