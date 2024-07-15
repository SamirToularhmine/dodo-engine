#include <DodoEngine/Core/Camera.h>

#include <DodoEngine/Core/InputManager.h>
#include <DodoEngine/Utils/Utils.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DODO_BEGIN_NAMESPACE

Camera::Camera(glm::vec3 _position) : m_Position(_position), m_Pitch(0), m_Yaw(0), m_Roll(0), m_Distance(0) {}

void Camera::Update(float _deltaTime) {
  DODO_TRACE(Camera);
  InputManager& inputManager = InputManager::Get();

  const double mouseDeltaScrollY = inputManager.GetScrollDeltaY();
  if (mouseDeltaScrollY != 0) {
    m_Distance -= mouseDeltaScrollY * MOUSE_SENSITIVITY * _deltaTime;
  }

  if (inputManager.IsMiddleMouseDown()) {
    const double mouseDeltaX = inputManager.GetMouseDeltaX();
    if (mouseDeltaX != 0) {
      m_Yaw -= mouseDeltaX * MOUSE_SENSITIVITY * _deltaTime;
    }

    const double mouseDeltaY = inputManager.GetMouseDeltaY();
    if (mouseDeltaY != 0) {
      m_Pitch -= mouseDeltaY * MOUSE_SENSITIVITY * _deltaTime;
    }
  }

  inputManager.ResetMouseDelta();
}

glm::mat4 Camera::GetViewMatrix() const {
  glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -m_Distance));
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Yaw), glm::vec3(0, 1, 0));
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Pitch), glm::vec3(1, 0, 0));
  return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix(const glm::vec2& extent) const {
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), extent.x / static_cast<float>(extent.y), NEAR_PLANE, FAR_PLANE);
  projectionMatrix[1][1] *= -1;

  return projectionMatrix;
}

DODO_END_NAMESPACE
