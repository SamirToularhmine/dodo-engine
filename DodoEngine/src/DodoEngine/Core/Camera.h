#pragma once

#include <DodoEngine/Core/Types.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DODO_BEGIN_NAMESPACE

struct CameraPerformanceStats {
  float m_UpdateFrameTime{0.0f};
};

struct Quaternion {
  float m_Cosine;
  glm::vec3 m_Axis;
};

class Camera {
 public:
  static constexpr float FAR_PLANE = 100.0f;
  static constexpr float NEAR_PLANE = 0.01f;
  static CameraPerformanceStats s_CameraPerformanceStats;

  Camera(glm::vec3 _position);

  ~Camera() = default;

  void Update(float _deltaTime);
  void Reset();

  const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
  const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };

  void UpdateProjectionMatrix(const glm::vec2& extent);

  static CameraPerformanceStats GetPerformanceStats() { return s_CameraPerformanceStats; };

 private:
  static constexpr double MOUSE_SENSITIVITY = 100.0;

  glm::vec3 m_Position;

  float m_Pitch{0.0f};
  float m_Yaw{0.0f};
  float m_Roll{0.0f};

  glm::mat4 m_ProjectionMatrix;
  glm::mat4 m_ViewMatrix;
};

DODO_END_NAMESPACE