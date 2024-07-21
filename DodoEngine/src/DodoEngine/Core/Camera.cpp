#include <DodoEngine/Core/Camera.h>

#include <DodoEngine/Core/InputManager.h>
#include <DodoEngine/Core/Key.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Utils/Utils.h>

#define _USE_MATH_DEFINES
#include <math.h>

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
    float rho = sqrt((m_Position.x * m_Position.x) + (m_Position.y * m_Position.y) + (m_Position.z * m_Position.z));
    float theta = acos(m_Position.y / rho);
    float phi = atan2(m_Position.z, m_Position.x);

    rho -= mouseDeltaScrollY * MOUSE_SENSITIVITY * _deltaTime;

    m_Position = glm::vec3{rho * sin(theta) * cos(phi), rho * cos(theta), rho * sin(theta) * sin(phi)};
  }

  if (inputManager.IsMiddleMouseReleased()) {
    /*m_LastQuaternion.m_Axis = m_RotationalAxis2;
    m_LastQuaternion.m_Cosine = m_CosValue2;*/
  }

  if (inputManager.IsMiddleMousePressed()) {
    /* m_StartMousePosX = ((inputManager.GetMousePosX() - (extent.width / 2)) / (extent.width / 2)) * radius;
    m_StartMousePosY = (((extent.height / 2) - inputManager.GetMousePosY()) / (extent.height / 2)) * radius;
    const float m_StartMousePosX2 = m_StartMousePosX * m_StartMousePosX;
    const float m_StartMousePosY2 = m_StartMousePosY * m_StartMousePosY;
    m_StartMousePosZ = m_StartMousePosX2 + m_StartMousePosY2 <= radius ? std::sqrt(radius - m_StartMousePosX2 - m_StartMousePosY2) : 0;*/

    const float mousePosX = inputManager.GetMousePosX();
    const float mousePosY = inputManager.GetMousePosY();

    glm::vec3 projectedMousePos;
    projectedMousePos.x = (mousePosX - (extent.width / 2)) / radius;
    projectedMousePos.y = (mousePosY - (extent.width / 2)) / radius;

    const float r = projectedMousePos.x * projectedMousePos.x + projectedMousePos.y * projectedMousePos.y;

    projectedMousePos.z = sqrt(1 - r);

    if (r > 0) {
      const float s = 1 / sqrt(r);
      projectedMousePos.x *= s;
      projectedMousePos.y *= s;
      projectedMousePos.z = 0;
    }
  }

  if (inputManager.IsMiddleMouseDown()) {
    /*const float currentMousePosX = ((inputManager.GetMousePosX() - (extent.width / 2)) / (extent.width / 2)) * radius;
    const float currentMousePosY = (((extent.height / 2) - inputManager.GetMousePosY()) / (extent.height / 2)) * radius;
    const float currentMousePosX2 = currentMousePosX * currentMousePosX;
    const float currentMousePosY2 = currentMousePosY * currentMousePosY;
    const float currentMousePosZ = currentMousePosX2 + currentMousePosY2 <= radius ? std::sqrt(radius - currentMousePosX2 - currentMousePosY2) : 0;

    const glm::vec3 startPos{m_StartMousePosX, m_StartMousePosY, m_StartMousePosZ};
    const glm::vec3 currentPos{currentMousePosX, currentMousePosY, currentMousePosZ};

    const glm::vec3 startPosUnitVector = glm::normalize(startPos);
    const glm::vec3 currentPosUnitVector = glm::normalize(currentPos);

    m_CurrentQuaternion.m_Axis = glm::cross(startPos, currentPos);
    m_CurrentQuaternion.m_Axis = glm::normalize(m_CurrentQuaternion.m_Axis);

    float cosValue = glm::dot(startPosUnitVector, currentPosUnitVector);
    if (cosValue > 1)
      cosValue = 1;
    const float theta = (std::acos(cosValue) * 180 / M_PI);

    m_CurrentQuaternion.m_Cosine = std::cos((theta / 2) * M_PI / 180);
    m_CurrentQuaternion.m_Axis.x = m_CurrentQuaternion.m_Axis.x * sin((theta / 2) * M_PI / 180);
    m_CurrentQuaternion.m_Axis.y = m_CurrentQuaternion.m_Axis.y * sin((theta / 2) * M_PI / 180);
    m_CurrentQuaternion.m_Axis.z = m_CurrentQuaternion.m_Axis.z * sin((theta / 2) * M_PI / 180);

    m_CosValue2 = (m_CurrentQuaternion.m_Cosine * m_LastQuaternion.m_Cosine) - glm::dot(m_CurrentQuaternion.m_Axis, m_LastQuaternion.m_Axis);

    glm::vec3 temporaryVector = glm::cross(m_CurrentQuaternion.m_Axis, m_LastQuaternion.m_Axis);

    m_RotationalAxis2.x =
        (m_CurrentQuaternion.m_Cosine * m_LastQuaternion.m_Axis.x) + (m_LastQuaternion.m_Cosine * m_CurrentQuaternion.m_Axis.x) + temporaryVector.x;
    m_RotationalAxis2.y =
        (m_CurrentQuaternion.m_Cosine * m_LastQuaternion.m_Axis.y) + (m_LastQuaternion.m_Cosine * m_CurrentQuaternion.m_Axis.y) + temporaryVector.y;
    m_RotationalAxis2.z =
        (m_CurrentQuaternion.m_Cosine * m_LastQuaternion.m_Axis.z) + (m_LastQuaternion.m_Cosine * m_CurrentQuaternion.m_Axis.z) + temporaryVector.z;

    m_Angle = (std::acos(m_CosValue2) * 180 / M_PI) * 2;
    m_RotationalAxis = {m_RotationalAxis2.x / sin((m_Angle / 2) * M_PI / 180), m_RotationalAxis2.y / sin((m_Angle / 2) * M_PI / 180),
                        m_RotationalAxis2.z / sin((m_Angle / 2) * M_PI / 180)};*/
  }
}

void Camera::Reset() {
  InputManager& inputManager = InputManager::Get();
  inputManager.Reset();
}

glm::mat4 Camera::GetViewMatrix() const {
  glm::mat4 viewMatrix(1.0f);
  viewMatrix = glm::translate(viewMatrix, m_Position);
  viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Angle), m_RotationalAxis);
  return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix(const glm::vec2& extent) const {
  glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), extent.x / static_cast<float>(extent.y), NEAR_PLANE, FAR_PLANE);
  projectionMatrix[1][1] *= -1;

  return projectionMatrix;
}

DODO_END_NAMESPACE
