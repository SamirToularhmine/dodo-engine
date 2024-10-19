#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Model.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DODO_BEGIN_NAMESPACE

struct TransformComponent {

  glm::vec3 m_Position;
  glm::mat4 m_TransformMatrix;

  TransformComponent(const glm::vec3& _position) : m_Position(_position) {
    glm::mat4 viewMatrix{1.0f};
    m_TransformMatrix = glm::translate(viewMatrix, _position);
  }
};

struct MeshComponent {
  Ref<Model> m_Model;
};

DODO_END_NAMESPACE