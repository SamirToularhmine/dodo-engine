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

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix(const glm::vec2& extent) const;

  static CameraPerformanceStats GetPerformanceStats() { return s_CameraPerformanceStats; };

 private:
  static constexpr double MOUSE_SENSITIVITY = 100.0;

  glm::vec3 m_Position;

  float m_Pitch{0.0f};
  float m_Yaw{0.0f};
  float m_Roll{0.0f};
};

DODO_END_NAMESPACE