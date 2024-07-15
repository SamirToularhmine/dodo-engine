#pragma once

#include <DodoEngine/Core/Types.h>

#include <glm/glm.hpp>

DODO_BEGIN_NAMESPACE

struct CameraPerformanceStats {
  float m_UpdateFrameTime{0.0f};
};

class Camera {
 public:
  static constexpr float FAR_PLANE = 100.0f;
  static constexpr float NEAR_PLANE = 0.01f;
  static CameraPerformanceStats s_CameraPerformanceStats;

  Camera(glm::vec3 _position);

  ~Camera() = default;

  void Update(float _deltaTime);

  glm::mat4 GetViewMatrix() const;

  glm::mat4 GetProjectionMatrix(const glm::vec2& extent) const;

  static CameraPerformanceStats GetPerformanceStats() { return s_CameraPerformanceStats; };

 private:
  static constexpr double MOUSE_SENSITIVITY = 10.0;

  glm::vec3 m_Position;

  float m_Pitch;
  float m_Yaw;
  float m_Roll;

  float m_Distance;
};

DODO_END_NAMESPACE