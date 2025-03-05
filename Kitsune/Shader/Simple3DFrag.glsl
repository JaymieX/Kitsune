#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionMatrix;
	mat4 ViewMatrix;
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
	vec3 directionToPointLight = ubo.pointLightPosition - fragPosWorld;

	float attenuation = 1.0 / dot(directionToPointLight, directionToPointLight); // distance squared cheez

	vec3 pointLightColor = ubo.pointLightColor.xyz * ubo.pointLightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientColor.xyz * ubo.ambientColor.w;

	vec3 diffuseLight = pointLightColor * max(dot(normalize(fragNormalWorld), normalize(directionToPointLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}