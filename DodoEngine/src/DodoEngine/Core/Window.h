#pragma once

#include <DodoEngine/Core/Types.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

DODO_BEGIN_NAMESPACE

struct WindowProps
{
  uint32_t m_Width{1920};
  uint32_t m_Height{1080};
  std::string m_Name{"Dodo Engine"};
  bool m_VSync{false};
  bool m_FullScreen{false};
};

class Window
{
public:
  void Init(const WindowProps &_windowProps);

  bool ShouldClose() const;

  void SwapBuffers() const;

  void PollEvents() const;

  void Update() const;

  void Shutdown() const;

  float GetTime() const;

  GLFWwindow *GetNativeWindow() const { return m_NativeWindow; };

private:
  WindowProps m_Props{};
  GLFWwindow *m_NativeWindow{nullptr};
};

DODO_END_NAMESPACE