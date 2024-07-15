#version 450

layout (constant_id = 0) const int MAX_MODEL_COUNT = 1022;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model[MAX_MODEL_COUNT];
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform RendererPushConstants
{
    int modelId;
    int textureId;
} PushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) out int fragTextureId;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoords = inTexCoords;
    fragTextureId = PushConstants.textureId;
}