#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform KitPushConstantsData {
	mat4 transform;
	mat4 normalMatrix;
} kitPushConstantsData;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT_LIGHT = 0.02;

void main() {
	gl_Position = kitPushConstantsData.transform * vec4(position, 1.0);

	// No need for 4x4 since normal is just a direction
	// mat3 normalMatrix = transpose(inverse(mat3(kitPushConstantsData.model)));
	// vec3 normalWorldSpace = normalize(normalMatrix * normal);

	vec3 normalWorldSpace = normalize(mat3(kitPushConstantsData.normalMatrix) * normal);

	float lightIntensity = AMBIENT_LIGHT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

	fragColor = lightIntensity * color;
}