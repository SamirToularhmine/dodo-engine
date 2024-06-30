#pragma once

#include <DodoEngine/Core/Types.h>

#include <glm/glm.hpp>


DODO_BEGIN_NAMESPACE

struct MeshTransform
{
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;
    glm::vec3 m_Scale;

    glm::mat4 GetModelMatrix()
    {
        const 
    }
};

DODO_END_NAMESPACE