#pragma once

#include <DodoEngine/Core/Types.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


DODO_BEGIN_NAMESPACE

struct MeshTransform
{
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;

    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_Position);

        modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
        
        return glm::scale(modelMatrix, m_Scale);
    }
};

DODO_END_NAMESPACE