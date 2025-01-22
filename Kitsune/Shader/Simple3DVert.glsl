#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform KitPushConstantsData {
	mat4 transform;
	vec3 color;
} kitPushConstantsData;

void main() {
	gl_Position = kitPushConstantsData.transform * vec4(position, 1.0);
	fragColor = color;
}