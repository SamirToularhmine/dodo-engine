#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/InputManager.h>
#include <DodoEngine/Core/Key.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanSwapChain.h>
#include <DodoEngine/Utils/Utils.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DODO_BEGIN_NAMESPACE

Camera::Camera(glm::vec3 _position)
    : m_Position(_position), m_ProjectionMatrix(1.0f), m_ViewMatrix(1.0f)
{
}

void Camera::Update(float _deltaTime)
{
  DODO_TRACE(Camera);

  InputManager &inputManager = InputManager::Get();
  constexpr float radius = 1.0f;

  const double mouseDeltaScrollY = inputManager.GetScrollDeltaY();
  if (mouseDeltaScrollY != 0)
  {
    m_Position.z += mouseDeltaScrollY * MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_D))
  {
    m_Yaw += -MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_A))
  {
    m_Yaw += MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_W))
  {
    m_Pitch += MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_S))
  {
    m_Pitch += -MOUSE_SENSITIVITY * _deltaTime;
  }

  m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position);
  m_ViewMatrix =
      glm::rotate(m_ViewMatrix, glm::radians(m_Pitch), glm::vec3(1, 0, 0));
  m_ViewMatrix =
      glm::rotate(m_ViewMatrix, glm::radians(m_Yaw), glm::vec3(0, 1, 0));
  m_ViewMatrix =
      glm::rotate(m_ViewMatrix, glm::radians(m_Roll), glm::vec3(0, 0, 1));
}

void Camera::Reset()
{
  InputManager &inputManager = InputManager::Get();
  inputManager.Reset();
}

void Camera::UpdateProjectionMatrix(const glm::vec2 &extent)
{
  m_ProjectionMatrix = glm::perspective(glm::radians(90.0f),
                                        extent.x / static_cast<float>(extent.y),
                                        NEAR_PLANE, FAR_PLANE);
  m_ProjectionMatrix[1][1] *= -1;
}

DODO_END_NAMESPACE
