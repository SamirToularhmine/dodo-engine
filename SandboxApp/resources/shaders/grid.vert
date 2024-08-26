#version 450

// To fix
layout (constant_id = 0) const int MAX_MODEL_COUNT = 1021;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model[MAX_MODEL_COUNT];
    mat4 view;
    mat4 proj;
    vec3 lightPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out float near;
layout(location = 1) out float far;
layout(location = 2) out vec3 nearPoint;
layout(location = 3) out vec3 farPoint;
layout(location = 4) out mat4 fragView;
layout(location = 8) out mat4 fragProj;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    near = 0.01f;
    far = 100.0f;
    vec3 p = inPosition.xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, ubo.view, ubo.proj).xyz;
    farPoint = UnprojectPoint(p.x, p.y, 1.0, ubo.view, ubo.proj).xyz;
    gl_Position = vec4(p, 1.0);
    fragProj = ubo.proj;
    fragView = ubo.view;
}