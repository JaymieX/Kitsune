#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(fragment)

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform KitPushConstantsData {
	vec2 offset;
	vec3 color;
} kitPushConstantsData;

void main() {
	outColor = vec4(kitPushConstantsData.color, 1.0);
}