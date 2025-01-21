#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
	outColor = color;
}