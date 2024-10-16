#version 450

layout(binding = 1) uniform sampler2D texSamplers[128];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) flat in int textureId;
layout(location = 3) in vec3 lightPos;
layout(location = 4) in vec3 fragNormals;
layout(location = 5) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 warmColor = vec3(0.3f, 0.3f, 0.0f) + 0.25f * fragColor;

vec3 lit(vec3 lightPos, vec3 normals, vec3 vertexPos)
{
	vec3 rl = reflect(-lightPos, normals);
	float s = clamp(100.0f * dot(rl, vertexPos) - 97.0f, 0.0f, 1.0f);
	vec3 highlightColor = vec3(2, 2, 2);
	return mix(warmColor, highlightColor, s);
}

void main() {
	outColor = texture(texSamplers[textureId], fragTexCoords);
	// outColor = vec4(fragColor, 1.0f);

	vec3 normalsNormalized = normalize(fragNormals);
	vec3 lightPosNormalized = normalize(lightPos - fragPos);
	float NdL = clamp(dot(normalsNormalized, lightPosNormalized), 0.0f, 1.0f);
	outColor.rgb += NdL * vec3(1, 1, 1) * lit(lightPosNormalized, normalsNormalized, fragPos);
}
