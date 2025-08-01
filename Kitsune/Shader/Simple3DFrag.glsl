#version 460

#extension GL_KHR_vulkan_glsl : enable
#pragma shader_stage(fragment)

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct PointLight {
	vec4 position; // ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionMatrix;
	mat4 ViewMatrix;
	mat4 invView;
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
	vec3 diffuseLight = ubo.ambientColor.xyz * ubo.ambientColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];

		vec3 directionToLight = light.position.xyz - fragPosWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared cheez
		directionToLight = normalize(directionToLight);

		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;

		// specular lighting
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
		specularLight += intensity * blinnTerm;
	}

	outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}