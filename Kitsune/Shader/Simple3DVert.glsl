#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

struct PointLight {
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionMatrix;
	mat4 ViewMatrix;
	vec3 directionToLight;
	vec4 ambientColor;
	PointLight pointLights[10];
	int numLights;
} ubo;

layout(push_constant) uniform KitPushConstantsData {
	mat4 modelMatrix;
	mat4 normalMatrix;
} kitPushConstantsData;

void main() {
	vec4 worldPosition = kitPushConstantsData.modelMatrix * vec4(position, 1.0);

	gl_Position = ubo.projectionMatrix * ubo.ViewMatrix * worldPosition;

	// No need for 4x4 since normal is just a direction
	// mat3 normalMatrix = transpose(inverse(mat3(kitPushConstantsData.model)));
	// vec3 normalWorldSpace = normalize(normalMatrix * normal);

	fragNormalWorld = normalize(mat3(kitPushConstantsData.normalMatrix) * normal);
	fragPosWorld = worldPosition.xyz;
	fragColor = color;
}