#version 450

layout(binding = 1) uniform sampler2D texSamplers[128];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) flat in int textureId;
layout(location = 3) in vec3 lightPos;
layout(location = 4) in vec3 fragNormals;
layout(location = 5) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main() {
	// outColor = texture(texSamplers[textureId], fragTexCoords);
	vec3 cCool = vec3(0, 0, 0.55f) + 0.25f * vec3(1, 0, 0);
	vec3 cWarm = vec3(0.3f, 0.3f, 0) + 0.25f * vec3(1, 0, 0);
	vec3 cHighlight = vec3(1, 1, 1);

	float nl = dot(fragNormals, lightPos);
	float t = (nl + 1) / 2;
	vec3 r = 2 * nl * fragNormals - lightPos;
	float s = 100 * dot(r, fragPos) - 97;

	vec3 cShaded = s * cHighlight + (1 - s) * (t * cWarm + (1 - t) * cCool);

	outColor = vec4(cShaded.xyz, 1.0f);
}