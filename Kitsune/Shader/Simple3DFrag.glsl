#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(fragment)

layout (location = 0) in vec3 fragColor;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform KitPushConstantsData {
	mat4 transform;
	mat4 normalMatrix;
} kitPushConstantsData;

void main() {
	outColor = vec4(fragColor, 1.0);
}