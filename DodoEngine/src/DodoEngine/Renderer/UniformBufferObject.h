#pragma once

#include <DodoEngine/Renderer/Renderer.h>

#include <glm/glm.hpp>

DODO_BEGIN_NAMESPACE

struct UniformBufferObject {
    glm::mat4 m_ModelTransform[Renderer::MAX_MODEL_COUNT];
    glm::mat4 m_View;
    glm::mat4 m_Proj;
    glm::vec3 m_LightPos;
};

DODO_END_NAMESPACE
