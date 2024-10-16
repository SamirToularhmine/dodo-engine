
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

Camera::Camera(glm::vec3 _position) : m_Position(_position) {}

void Camera::Update(float _deltaTime) {
  DODO_TRACE(Camera);

  InputManager& inputManager = InputManager::Get();
  constexpr float radius = 1.0f;
  const VulkanSwapChainData& swapChainData = VulkanContext::Get().GetSwapChain()->GetSpec();
  const VkExtent2D& extent = swapChainData.m_VkExtent;

  const double mouseDeltaScrollY = inputManager.GetScrollDeltaY();
  if (mouseDeltaScrollY != 0) {
    m_Position.z += mouseDeltaScrollY * MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_D)) {
    m_Yaw += -MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_A)) {
    m_Yaw += MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_W)) {
    m_Pitch += MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsKeyDown(Key::DODO_KEY_S)) {
    m_Pitch += -MOUSE_SENSITIVITY * _deltaTime;
  }
}

void Camera::Reset() {
  InputManager& inputManager = InputManager::Get();
  inputManager.Reset();
}

glm::mat4 Camera::GetViewMatrix() const {
  glm::mat4 viewMatrix(1.0f);
  viewMatrix = glm::translate(viewMatrix, m_Position);
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Pitch), glm::vec3(1, 0, 0));
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Yaw), glm::vec3(0, 1, 0));
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Roll), glm::vec3(0, 0, 1));
  return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix(const glm::vec2& extent) const {
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), extent.x / static_cast<float>(extent.y), NEAR_PLANE, FAR_PLANE);
  projectionMatrix[1][1] *= -1;

  return projectionMatrix;
}

DODO_END_NAMESPACE
