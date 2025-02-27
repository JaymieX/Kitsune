#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionViewMatrix;
	vec3 directionToLight;
	vec4 ambientColor;
	vec3 pointLightPosition;
	vec4 pointLightColor;
} ubo;

layout(push_constant) uniform KitPushConstantsData {
	mat4 modelMatrix;
	mat4 normalMatrix;
} kitPushConstantsData;

void main() {
	vec4 worldPosition = kitPushConstantsData.modelMatrix * vec4(position, 1.0);

	gl_Position = ubo.projectionViewMatrix * worldPosition;

	// No need for 4x4 since normal is just a direction
	// mat3 normalMatrix = transpose(inverse(mat3(kitPushConstantsData.model)));
	// vec3 normalWorldSpace = normalize(normalMatrix * normal);

	vec3 normalWorldSpace = normalize(mat3(kitPushConstantsData.normalMatrix) * normal);

	vec3 directionToPointLight = ubo.pointLightPosition - worldPosition.xyz;

	float attenuation = 1.0 / dot(directionToPointLight, directionToPointLight); // distance squared cheez

	vec3 pointLightColor = ubo.pointLightColor.xyz * ubo.pointLightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientColor.xyz * ubo.ambientColor.w;

	vec3 diffuseLight = pointLightColor * max(dot(normalWorldSpace, normalize(directionToPointLight)), 0);

	// float lightIntensity = AMBIENT_LIGHT + max(dot(normalWorldSpace, ubo.directionToLight), 0);

	fragColor = (diffuseLight + ambientLight) * color;
}