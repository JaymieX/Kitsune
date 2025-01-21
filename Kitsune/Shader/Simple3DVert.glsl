#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform KitPushConstantsData {
	vec2 offset;
	vec3 color;
} kitPushConstantsData;

void main() {
	gl_Position = vec4(position + kitPushConstantsData.offset, 0.0, 1.0);
}