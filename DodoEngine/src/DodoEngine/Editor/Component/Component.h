#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Model.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DODO_BEGIN_NAMESPACE

class Entity;

struct EntityComponent
{
  entt::entity m_EditorEntity;
  Ref<Entity> m_Entity;
};

struct TransformComponent
{
  glm::vec3 m_Position;
  glm::vec3 m_Rotation;
  glm::vec3 m_Scale;

  glm::mat4 m_TransformMatrix;

  TransformComponent(const glm::vec3 &_position, const glm::vec3 &_rotation,
                     const glm::vec3 &_scale)
      : m_Position(_position), m_Rotation(_rotation), m_Scale(_scale)
  {
    UpdateTransformMatrix();
  }

  void UpdateTransformMatrix()
  {
    glm::mat4 viewMatrix{1.0f};

    m_TransformMatrix = glm::translate(viewMatrix, m_Position);
    m_TransformMatrix =
        glm::rotate(m_TransformMatrix, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
    m_TransformMatrix =
        glm::rotate(m_TransformMatrix, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
    m_TransformMatrix =
        glm::rotate(m_TransformMatrix, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
    m_TransformMatrix = glm::scale(m_TransformMatrix, m_Scale);
  }
};

struct MeshComponent
{
  Ref<Model> m_Model;
};

DODO_END_NAMESPACE