#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

const vec2 OFFSETS[6] = vec2[](
vec2(-1.0, -1.0),
vec2(-1.0, 1.0),
vec2(1.0, -1.0),
vec2(1.0, -1.0),
vec2(-1.0, 1.0),
vec2(1.0, 1.0)
);

layout(location = 0) out vec2 fragOffset;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 projectionMatrix;
    mat4 ViewMatrix;
    vec3 directionToLight;
    vec4 ambientColor;
    vec3 pointLightPosition;
    vec4 pointLightColor;
} ubo;

const float LIGHT_RADIUS = 0.1;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];

    vec3 cameraRightWorld = {ubo.ViewMatrix[0][0], ubo.ViewMatrix[1][0], ubo.ViewMatrix[2][0]};
    vec3 cameraUpWorld = {ubo.ViewMatrix[0][1], ubo.ViewMatrix[1][1], ubo.ViewMatrix[2][1]};

    vec3 positionWorld = ubo.pointLightPosition.xyz
    + LIGHT_RADIUS * fragOffset.x * cameraRightWorld
    + LIGHT_RADIUS * fragOffset.y * cameraUpWorld;

    gl_Position = ubo.projectionMatrix * ubo.ViewMatrix * vec4(positionWorld, 1.0);
}