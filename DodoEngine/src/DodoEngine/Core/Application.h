#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Editor/ImGuiLayer.h>
#include <DodoEngine/Renderer/Renderer.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Application {
 public:
  static Ptr<Application> Create();

  void Init(const WindowProps& _windowProps);

  void Run();

 private:
  Ptr<Window> m_Window;
  Ptr<Renderer> m_Renderer;
  Ptr<Camera> m_Camera;
  Ptr<ImGuiLayer> m_ImGuiLayer;

  float m_LastFrameTime;
};

DODO_END_NAMESPACE