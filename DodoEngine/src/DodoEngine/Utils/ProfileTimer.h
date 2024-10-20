#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Debug/PerformanceManager.h>

#include <GLFW/glfw3.h>

#include <string>


DODO_BEGIN_NAMESPACE

class ProfileTimer {
 public:
  ProfileTimer(const std::string& _name) : m_InitTime(glfwGetTime()), m_Name(_name) {}

  ~ProfileTimer() { PerformanceManager::Store(m_Name, GetTime()); }

  float GetTime() const { return glfwGetTime() - m_InitTime; }

 private:
  std::string m_Name;
  float m_InitTime;
};

DODO_END_NAMESPACE