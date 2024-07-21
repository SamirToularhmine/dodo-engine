#version 450

layout(binding = 1) uniform sampler2D texSamplers[128];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoords;
layout(location = 2) flat in int textureId;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSamplers[textureId], fragTexCoords);
}